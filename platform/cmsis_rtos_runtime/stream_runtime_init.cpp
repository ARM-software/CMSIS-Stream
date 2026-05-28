#include "stream_platform_config.hpp"
#include "cg_enums.h"
#include "EventQueue.hpp"
#include "StreamNode.hpp"

#include "stream_event_queue.hpp"

#include "stream_runtime_init.hpp"

#include "stream_rtos_events.h"

#include <atomic>

#include <new>

#include <memory>

#include "cmsis_compiler.h"





osEventFlagsId_t cg_streamEvent;
osEventFlagsId_t cg_streamReplyEvent;

using namespace arm_cmsis_stream;

// Define memory pool sizes
#define LIST_ELEMENT_SIZE (sizeof(ListValue) + sizeof(std::shared_ptr<ListValue>) + CMSISSTREAM_SHARED_OVERHEAD)
#define LIST_SIZE (CMSISSTREAM_NB_MAX_EVENTS * LIST_ELEMENT_SIZE)

#define BUF_ELEMENT_SIZE (sizeof(Tensor<double>) + sizeof(std::shared_ptr<Tensor<double>>) + CMSISSTREAM_SHARED_OVERHEAD)
#define BUF_SIZE (CMSISSTREAM_NB_MAX_BUFS * BUF_ELEMENT_SIZE)

#define MUTEX_ELEMENT_SIZE (sizeof(CG_MUTEX) + sizeof(std::shared_ptr<CG_MUTEX>) + CMSISSTREAM_SHARED_OVERHEAD)
#define MUTEX_SIZE (CMSISSTREAM_NB_MAX_BUFS * MUTEX_ELEMENT_SIZE)

__ALIGNED(8) __attribute__((section(CMSISSTREAM_POOL_SECTION))) static uint8_t list_slab_area[LIST_SIZE];

__ALIGNED(8) __attribute__((section(CMSISSTREAM_POOL_SECTION))) static uint8_t buf_slab_area[BUF_SIZE];

__ALIGNED(8) __attribute__((section(CMSISSTREAM_POOL_SECTION))) static uint8_t mutex_slab_area[MUTEX_SIZE];

osMemoryPoolId_t cg_eventPool;
osMemoryPoolId_t cg_bufPool;
osMemoryPoolId_t cg_mutexPool;

static osThreadId_t tid_stream = nullptr;
static osThreadId_t tid_event = nullptr;

// CMSIS-RTOS threads must be created with osThreadJoinable when the runtime
// later waits for them with osThreadJoin().
static const osThreadAttr_t stream_thread_attr = {
	    .attr_bits = osThreadJoinable,
        .stack_size = CMSISSTREAM_STREAM_THREAD_STACK_SIZE,
        .priority = CMSISSTREAM_STREAM_THREAD_PRIORITY
    };

static const osThreadAttr_t event_thread_attr = {
	    .attr_bits = osThreadJoinable,
        .stack_size = CMSISSTREAM_EVT_THREAD_STACK_SIZE,
        .priority = CMSISSTREAM_EVT_HIGH_PRIORITY,
    };


using namespace arm_cmsis_stream;

std::atomic<stream_execution_context_t *> current_context = nullptr;
static std::atomic<bool> runtime_stop_requested = false;

static void set_current_context(stream_execution_context_t *context)
{
	current_context.store(context);
}

static void cleanup_started_event_thread()
{
	if (tid_event != nullptr)
	{
		osThreadTerminate(tid_event);
		osThreadJoin(tid_event);
		tid_event = nullptr;
	}
	set_current_context(nullptr);
}

static bool is_runtime_scheduler_error(int error)
{
	return ((error != CG_SUCCESS) &&
		(error != CG_STOP_SCHEDULER) &&
		(error != CG_PAUSED_SCHEDULER));
}

static void report_runtime_error(cg_error_origin origin, cg_status status,
				 int32_t src_node_id, int32_t info)
{
	EventQueue::callSyncHandler(src_node_id,
				    Event(kError, kHighPriority,
					  static_cast<int32_t>(origin),
					  static_cast<int32_t>(status),
					  info));
}

static void report_stop_graph()
{
	EventQueue::callSyncHandler(CG_UNIDENTIFIED_NODE,
				    Event(kStopGraph, kHighPriority));
}

static void wait_stream_resume(stream_execution_context_t *context)
{
	while (true)
	{
		uint32_t res = osEventFlagsWait(cg_streamEvent, STREAM_RESUME_EVENT,
						osFlagsWaitAny, osWaitForever);
		if ((res & STREAM_RESUME_EVENT) == 0)
		{
			continue;
		}

		if (runtime_stop_requested.load())
		{
			return;
		}

		// The context may have changed after a resume event
		context = current_context.load();
		// Clear FIFOs here. In case of memory overlay between different graphs, the
		// data in FIFOs could be corrupted when resuming another graph
		context->reset_fifos(1);
		if (context->resume_all_nodes)
			context->resume_all_nodes(context);
		osEventFlagsSet(cg_streamReplyEvent, STREAM_RESUMED_EVENT);

		CMSISSTREAM_LOG_DBG("Scheduler resumed\n");
		return;
	}
}

static void wait_event_thread_paused()
{
	(void)osEventFlagsWait(cg_streamReplyEvent, EVENT_PAUSED_EVENT,
			       osFlagsWaitAny, osWaitForever);
}

static void wait_event_resume()
{
	while (true)
	{
		uint32_t res = osEventFlagsWait(cg_streamEvent, EVENT_RESUME_EVENT,
						osFlagsWaitAny, osWaitForever);
		if ((res & EVENT_RESUME_EVENT) == 0)
		{
			continue;
		}
		osEventFlagsSet(cg_streamReplyEvent, EVENT_RESUMED_EVENT);
		return;
	}
}

static void pause_stream_thread_on_error(stream_execution_context_t *context, cg_status status)
{
	CMSISSTREAM_LOG_DBG("Try to pause event queue after stream error\n");
	context->evtQueue->pause();

	// The test is > 0 because when there are no dataflow nodes, the
	// pausing is done in the event thread with a corresponding test on == 0.
	if (context->scheduler_length > 0)
	{
		if (context->pause_all_nodes)
			context->pause_all_nodes(context);
	}

	wait_event_thread_paused();
	report_runtime_error(kStreamThread, status, CG_UNIDENTIFIED_NODE, 0);
	if (runtime_stop_requested.load())
	{
		return;
	}
	osEventFlagsSet(cg_streamReplyEvent, STREAM_PAUSED_EVENT);
	wait_stream_resume(context);
}

static void pause_stream_thread_on_stop_graph(stream_execution_context_t *context)
{
	CMSISSTREAM_LOG_DBG("Try to pause event queue after stop graph\n");
	context->evtQueue->pause();

	if (context->pause_all_nodes)
		context->pause_all_nodes(context);

	wait_event_thread_paused();
	report_stop_graph();
	if (runtime_stop_requested.load())
	{
		return;
	}
	osEventFlagsSet(cg_streamReplyEvent, STREAM_PAUSED_EVENT);
	wait_stream_resume(context);
}

static void pause_peer_stream_thread(stream_execution_context_t *context)
{
	// The test is > 0 because when there are no dataflow nodes, the
	// pausing is done in the event thread with a corresponding test on == 0.
	if (context->scheduler_length > 0)
	{
		CMSISSTREAM_LOG_DBG("Try to pause stream scheduler after event error\n");
		osEventFlagsSet(cg_streamEvent, STREAM_PAUSE_EVENT);
		(void)osEventFlagsWait(cg_streamReplyEvent, STREAM_PAUSED_EVENT,
				       osFlagsWaitAny, osWaitForever);
	}
}

static void event_thread_function(void *arg)
{

	CMSISSTREAM_LOG_DBG("Started event thread\n");
	bool done = false;

	while (!done)
	{
		// Process current queue
		stream_execution_context_t *context = current_context.load();
		EventQueue *queue = context->evtQueue;
		queue->execute();
		if (queue->mustEnd())
		{
			done = true;
		}
		else // Paused queue
		{
			cg_status event_error;
			int32_t event_error_node;
			int32_t event_error_info;
			bool has_error = queue->consumeError(event_error, event_error_node,
							     event_error_info);
			if (has_error)
			{
				pause_peer_stream_thread(context);
			}

			// If no dataflow nodes, the nodes are paused here when the event thread is pausing,
			// otherwise they are paused in stream thread.
			if (context->scheduler_length == 0)
			{
				if (context->pause_all_nodes)
					context->pause_all_nodes(context);
			}
			if (has_error)
			{
				report_runtime_error(kEventThread, event_error,
						     event_error_node, event_error_info);
			}
			if (runtime_stop_requested.load())
			{
				return;
			}
			osEventFlagsSet(cg_streamReplyEvent, EVENT_PAUSED_EVENT);
			wait_event_resume();
		}
	}
}

static void stream_thread_function(void *)
{

	uint32_t nb_iter;
	int error;
	bool done = false;
	CMSISSTREAM_LOG_DBG("Stream thread started\n");

	while (!done)
	{
		stream_execution_context_t *context = current_context.load();
		nb_iter = context->dataflow_scheduler(&error);
		if (is_runtime_scheduler_error(error))
		{
			CMSISSTREAM_LOG_ERR("Scheduler error %d\n", error);
			pause_stream_thread_on_error(context, static_cast<cg_status>(error));
			if (runtime_stop_requested.load())
			{
				break;
			}
			continue;
		}
		if ((context->scheduler_length > 0) && (error == CG_STOP_SCHEDULER))
		{
			CMSISSTREAM_LOG_DBG("Scheduler requested stop graph\n");
			pause_stream_thread_on_stop_graph(context);
			if (runtime_stop_requested.load())
			{
				break;
			}
			continue;
		}
		// When there is no dataflow node, the data flow graph returns
		// CG_STOP_SCHEDULER immediately. This should be interpreted as a pause.
		if ((error == CG_PAUSED_SCHEDULER) ||
			((context->scheduler_length == 0) && (error == CG_STOP_SCHEDULER)))
		{

			// If there are data flow nodes, we prefer pausing the nodes
			// as soon as possible and not do it when event thread is pausing
			if (context->scheduler_length > 0)
			{
				if (context->pause_all_nodes)
					context->pause_all_nodes(context);
				// If scheduler_length == 0 then
				// the data flow has paused alone and not in
				// reaction to a pause event
				// So no need to acknowledge the pause event
				osEventFlagsSet(cg_streamReplyEvent, STREAM_PAUSED_EVENT);
			}

			
			wait_stream_resume(context);
			if (runtime_stop_requested.load())
			{
				break;
			}
		}
		else
		{
			done = true;
		}
	}
	CMSISSTREAM_LOG_DBG("Scheduler done after %d iterations\n", nb_iter);
}

void stream_pause_current_scheduler()
{
	CMSISSTREAM_LOG_DBG("Try to pause event queue\n");
	current_context.load()->evtQueue->pause();
	// If the graph is a pure event graph (scheduler_len == 0), the dataflow part
	// is already paused so no need to post a pause event
	uint32_t res;
	if (current_context.load()->scheduler_length > 0)
	{
		CMSISSTREAM_LOG_DBG("Try to pause stream scheduler\n");
		osEventFlagsSet(cg_streamEvent, STREAM_PAUSE_EVENT);
		res = osEventFlagsWait(cg_streamReplyEvent, STREAM_PAUSED_EVENT, osFlagsWaitAny, osWaitForever);
	}
	
	res = osEventFlagsWait(cg_streamReplyEvent, EVENT_PAUSED_EVENT, osFlagsWaitAny, osWaitForever);
	current_context.load()->evtQueue->clear();

	CMSISSTREAM_LOG_DBG("Stream scheduler and event queue paused\n");
}

bool stream_resume_scheduler(stream_execution_context_t *context)
{
	CMSISSTREAM_LOG_DBG("Resuming stream scheduler and event queue\n");
	if ((context == nullptr) || (context->evtQueue == nullptr))
	{
		CMSISSTREAM_LOG_ERR("Can't resume stream scheduler with invalid context\n");
		return false;
	}
	set_current_context(context);
	context->evtQueue->resume();
	osEventFlagsSet(cg_streamEvent, STREAM_RESUME_EVENT);
	uint32_t res = osEventFlagsWait(cg_streamReplyEvent,
					STREAM_RESUMED_EVENT | STREAM_PAUSED_EVENT,
					osFlagsWaitAny, osWaitForever);
	if ((res & STREAM_PAUSED_EVENT) != 0)
	{
		CMSISSTREAM_LOG_ERR("Stream thread failed to resume\n");
		context->evtQueue->pause();
		report_runtime_error(kStreamThread, CG_RESUME_FAILURE,
				     CG_UNIDENTIFIED_NODE, 0);
		return false;
	}
	CMSISSTREAM_LOG_DBG("Stream thread resumed\n");
	osEventFlagsSet(cg_streamEvent, EVENT_RESUME_EVENT);
	res = osEventFlagsWait(cg_streamReplyEvent,
			       EVENT_RESUMED_EVENT | EVENT_PAUSED_EVENT,
			       osFlagsWaitAny, osWaitForever);
	if ((res & EVENT_PAUSED_EVENT) != 0)
	{
		CMSISSTREAM_LOG_ERR("Event thread failed to resume\n");
		context->evtQueue->pause();
		report_runtime_error(kEventThread, CG_RESUME_FAILURE,
				     CG_UNIDENTIFIED_NODE, 0);
		if (context->scheduler_length > 0)
		{
			osEventFlagsSet(cg_streamEvent, STREAM_PAUSE_EVENT);
			(void)osEventFlagsWait(cg_streamReplyEvent, STREAM_PAUSED_EVENT,
					       osFlagsWaitAny, osWaitForever);
		}
		return false;
	}
	osEventFlagsClear(cg_streamReplyEvent, STREAM_PAUSED_EVENT | EVENT_PAUSED_EVENT);
	CMSISSTREAM_LOG_DBG("Stream scheduler and event queue resumed\n");
	return true;
}

int stream_init_memory()
{
	CMSISSTREAM_LOG_DBG("Initializing stream memory\n");
	/* Init events */
	cg_streamEvent = osEventFlagsNew(NULL);
	cg_streamReplyEvent = osEventFlagsNew(NULL);

	/* Init memory slabs */
    cg_eventPool = osMemoryPoolNew(CMSISSTREAM_NB_MAX_EVENTS, LIST_ELEMENT_SIZE, NULL);

	
	if (cg_eventPool == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to init event pool slab\n");
		return (-1);
	}

	cg_bufPool = osMemoryPoolNew(CMSISSTREAM_NB_MAX_BUFS, BUF_ELEMENT_SIZE, NULL);
	if (cg_bufPool == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to init buf pool slab\n");
		return (-1);
	}

	cg_mutexPool = osMemoryPoolNew(CMSISSTREAM_NB_MAX_BUFS, MUTEX_ELEMENT_SIZE, NULL);
	if (cg_mutexPool == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to init mutex pool slab\n");
		return (-1);
	}

	CMSISSTREAM_LOG_DBG("Stream memory initialized\n");

	return (0);
}

bool stream_start_threads(stream_execution_context_t *context)
{
	if ((context == nullptr) || (context->evtQueue == nullptr))
	{
		CMSISSTREAM_LOG_ERR("Can't start stream threads with invalid context\n");
		return false;
	}

	set_current_context(context);
	runtime_stop_requested.store(false);

    tid_event = osThreadNew(event_thread_function, NULL, &event_thread_attr);
	if (tid_event == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to start event thread\n");
		set_current_context(nullptr);
		return false;
	}

	tid_stream = osThreadNew(stream_thread_function, NULL, &stream_thread_attr);
	if (tid_stream == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to start stream thread\n");
		cleanup_started_event_thread();
		return false;
	}
	CMSISSTREAM_LOG_DBG("Stream runtime threads started\n");
	return true;
}

static void stop_thread(osThreadId_t &tid, bool callerIsRuntimeThread)
{
	if (tid == nullptr)
	{
		return;
	}

	osThreadId_t current = osThreadGetId();
	if ((current != nullptr) && (current == tid))
	{
		(void)callerIsRuntimeThread;
		tid = nullptr;
		return;
	}

	(void)osThreadJoin(tid);
	tid = nullptr;
}

void stream_stop_threads(bool callerIsRuntimeThread)
{
	runtime_stop_requested.store(true);

	stream_execution_context_t *context = current_context.load();
	if ((context != nullptr) && (context->evtQueue != nullptr))
	{
		context->evtQueue->end();
	}

	if (cg_streamEvent != nullptr)
	{
		osEventFlagsSet(cg_streamEvent, STREAM_DONE_EVENT | STREAM_RESUME_EVENT | EVENT_RESUME_EVENT);
	}
	if (cg_streamReplyEvent != nullptr)
	{
		osEventFlagsSet(cg_streamReplyEvent,
				STREAM_PAUSED_EVENT | STREAM_RESUMED_EVENT |
				EVENT_PAUSED_EVENT | EVENT_RESUMED_EVENT);
	}

	stop_thread(tid_event, callerIsRuntimeThread);
	stop_thread(tid_stream, callerIsRuntimeThread);
	set_current_context(nullptr);
}

void stream_wait_for_threads_end()
{
	if (tid_stream != nullptr)
	{
		osThreadJoin(tid_stream);
		tid_stream = nullptr;
	}
	if (tid_event != nullptr)
	{
		osThreadJoin(tid_event);
		tid_event = nullptr;
	}
}

void stream_free_memory()
{
    osMemoryPoolDelete(cg_eventPool);
    osMemoryPoolDelete(cg_bufPool);
    osMemoryPoolDelete(cg_mutexPool);

    osEventFlagsDelete(cg_streamEvent);
    osEventFlagsDelete(cg_streamReplyEvent);
}

EventQueue *stream_new_event_queue()
{
	CMSISEventQueue *queue = new (std::nothrow) CMSISEventQueue(CMSISSTREAM_EVT_LOW_PRIORITY,
																CMSISSTREAM_EVT_NORMAL_PRIORITY,
																CMSISSTREAM_EVT_HIGH_PRIORITY);
	return (static_cast<EventQueue *>(queue));
}
