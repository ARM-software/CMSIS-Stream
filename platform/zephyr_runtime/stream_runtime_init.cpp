#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(cmsisstream, CONFIG_CMSISSTREAM_LOG_LEVEL);

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

struct k_event cg_streamEvent;
struct k_event cg_streamReplyEvent;

using namespace arm_cmsis_stream;

// Define memory pool sizes
#define LIST_ELEMENT_SIZE (sizeof(ListValue) + sizeof(std::shared_ptr<ListValue>) + CONFIG_CMSISSTREAM_SHARED_OVERHEAD)
#define LIST_SIZE (CONFIG_CMSISSTREAM_NB_MAX_EVENTS * LIST_ELEMENT_SIZE)

#define BUF_ELEMENT_SIZE (sizeof(Tensor<double>) + sizeof(std::shared_ptr<Tensor<double>>) + CONFIG_CMSISSTREAM_SHARED_OVERHEAD)
#define BUF_SIZE (CONFIG_CMSISSTREAM_NB_MAX_BUFS * BUF_ELEMENT_SIZE)

#define MUTEX_ELEMENT_SIZE (sizeof(CG_MUTEX) + sizeof(std::shared_ptr<CG_MUTEX>) + CONFIG_CMSISSTREAM_SHARED_OVERHEAD)
#define MUTEX_SIZE (CONFIG_CMSISSTREAM_NB_MAX_BUFS * MUTEX_ELEMENT_SIZE)

__aligned(8) __attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t list_slab_area[LIST_SIZE];

__aligned(8) __attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t buf_slab_area[BUF_SIZE];

__aligned(8) __attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t mutex_slab_area[MUTEX_SIZE];

struct k_mem_slab cg_eventPool;
struct k_mem_slab cg_bufPool;
struct k_mem_slab cg_mutexPool;

static k_tid_t tid_stream = nullptr;
static k_tid_t tid_event = nullptr;

static struct k_thread stream_thread;
static struct k_thread event_thread;

static K_THREAD_STACK_DEFINE(event_thread_stack, CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE);
static K_THREAD_STACK_DEFINE(stream_thread_stack, CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE);

using namespace arm_cmsis_stream;

std::atomic<stream_execution_context_t *> current_context = nullptr;

static void set_current_context(stream_execution_context_t *context)
{
	current_context.store(context);
}

static void cleanup_started_event_thread()
{
	if (tid_event != nullptr)
	{
		k_thread_abort(tid_event);
		k_thread_join(&event_thread, K_FOREVER);
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
				 int32_t node_id, int32_t info)
{
	EventQueue::callSyncHandler(CG_UNIDENTIFIED_NODE,
				    Event(kError, kHighPriority,
					  static_cast<int32_t>(origin),
					  static_cast<int32_t>(status),
					  node_id,
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
		uint32_t res = k_event_wait(&cg_streamEvent, STREAM_RESUME_EVENT, false,
					    K_FOREVER);
		if ((res & STREAM_RESUME_EVENT) == 0)
		{
			continue;
		}

		// The context may have changed after a resume event
		context = current_context.load();
		k_event_clear(&cg_streamEvent, STREAM_RESUME_EVENT);
		// Clear FIFOs here. In case of memory overlay between different graphs, the
		// data in FIFOs could be corrupted when resuming another graph
		context->reset_fifos(1);
		if (context->resume_all_nodes)
			context->resume_all_nodes(context);
		k_event_post(&cg_streamReplyEvent, STREAM_RESUMED_EVENT);

		LOG_DBG("Scheduler resumed\n");
		return;
	}
}

static void wait_event_thread_paused()
{
	uint32_t res = k_event_wait(&cg_streamReplyEvent, EVENT_PAUSED_EVENT,
				    false, K_FOREVER);
	if ((res & EVENT_PAUSED_EVENT) != 0)
	{
		k_event_clear(&cg_streamReplyEvent, EVENT_PAUSED_EVENT);
	}
}

static void wait_event_resume()
{
	while (true)
	{
		uint32_t res = k_event_wait(&cg_streamEvent, EVENT_RESUME_EVENT,
					    false, K_FOREVER);
		if ((res & EVENT_RESUME_EVENT) == 0)
		{
			continue;
		}
		k_event_clear(&cg_streamEvent, EVENT_RESUME_EVENT);
		k_event_post(&cg_streamReplyEvent, EVENT_RESUMED_EVENT);
		return;
	}
}

static void pause_stream_thread_on_error(stream_execution_context_t *context, cg_status status)
{
	LOG_DBG("Try to pause event queue after stream error\n");
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
	k_event_post(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
	wait_stream_resume(context);
}

static void pause_stream_thread_on_stop_graph(stream_execution_context_t *context)
{
	LOG_DBG("Try to pause event queue after stop graph\n");
	context->evtQueue->pause();

	if (context->pause_all_nodes)
		context->pause_all_nodes(context);

	wait_event_thread_paused();
	report_stop_graph();
	k_event_post(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
	wait_stream_resume(context);
}

static void pause_peer_stream_thread(stream_execution_context_t *context)
{
	// The test is > 0 because when there are no dataflow nodes, the
	// pausing is done in the event thread with a corresponding test on == 0.
	if (context->scheduler_length > 0)
	{
		LOG_DBG("Try to pause stream scheduler after event error\n");
		k_event_post(&cg_streamEvent, STREAM_PAUSE_EVENT);
		uint32_t res = k_event_wait(&cg_streamReplyEvent, STREAM_PAUSED_EVENT,
					    false, K_FOREVER);
		if ((res & STREAM_PAUSED_EVENT) != 0)
		{
			k_event_clear(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
		}
	}
}

static void event_thread_function(void *, void *, void *)
{

	LOG_DBG("Started event thread\n");
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
			k_event_post(&cg_streamReplyEvent, EVENT_PAUSED_EVENT);
			wait_event_resume();
		}
	}
}

static void stream_thread_function(void *, void *, void *)
{

	uint32_t nb_iter;
	int error;
	bool done = false;
	LOG_DBG("Started stream thread\n");

	while (!done)
	{
		stream_execution_context_t *context = current_context.load();
		nb_iter = context->dataflow_scheduler(&error);
		if (is_runtime_scheduler_error(error))
		{
			LOG_ERR("Scheduler error %d\n", error);
			pause_stream_thread_on_error(context, static_cast<cg_status>(error));
			continue;
		}
		if ((context->scheduler_length > 0) && (error == CG_STOP_SCHEDULER))
		{
			LOG_DBG("Scheduler requested stop graph\n");
			pause_stream_thread_on_stop_graph(context);
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
				k_event_post(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
			}

			
			wait_stream_resume(context);
		}
		else
		{
			done = true;
		}
	}
	LOG_DBG("Scheduler done after %d iterations\n", nb_iter);
}

void stream_pause_current_scheduler()
{
	LOG_DBG("Try to pause event queue\n");
	current_context.load()->evtQueue->pause();
	// If the graph is a pure event graph (scheduler_len == 0), the dataflow part
	// is already paused so no need to post a pause event
	uint32_t res;
	if (current_context.load()->scheduler_length > 0)
	{
		LOG_DBG("Try to pause stream scheduler\n");
		k_event_post(&cg_streamEvent, STREAM_PAUSE_EVENT);
		res = k_event_wait(&cg_streamReplyEvent, STREAM_PAUSED_EVENT, false, K_FOREVER);
		if ((res & STREAM_PAUSED_EVENT) != 0)
		{
			k_event_clear(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
		}
	}
	
	res = k_event_wait(&cg_streamReplyEvent, EVENT_PAUSED_EVENT, false, K_FOREVER);
	if ((res & EVENT_PAUSED_EVENT) != 0)
	{
		k_event_clear(&cg_streamReplyEvent, EVENT_PAUSED_EVENT);
	}
	current_context.load()->evtQueue->clear();

	LOG_DBG("Stream scheduler and event queue paused\n");
}

bool stream_resume_scheduler(stream_execution_context_t *context)
{
	LOG_DBG("Resuming stream scheduler and event queue\n");
	if ((context == nullptr) || (context->evtQueue == nullptr))
	{
		LOG_ERR("Can't resume stream scheduler with invalid context\n");
		return false;
	}
	set_current_context(context);
	context->evtQueue->resume();
	k_event_post(&cg_streamEvent, STREAM_RESUME_EVENT);
	uint32_t res = k_event_wait(&cg_streamReplyEvent,
				    STREAM_RESUMED_EVENT | STREAM_PAUSED_EVENT, false,
				    K_FOREVER);
	if ((res & STREAM_PAUSED_EVENT) != 0)
	{
		k_event_clear(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
		LOG_ERR("Stream thread failed to resume\n");
		context->evtQueue->pause();
		report_runtime_error(kStreamThread, CG_RESUME_FAILURE,
				     CG_UNIDENTIFIED_NODE, 0);
		return false;
	}
	if ((res & STREAM_RESUMED_EVENT) != 0)
	{
		k_event_clear(&cg_streamReplyEvent, STREAM_RESUMED_EVENT);
	}
	LOG_DBG("Stream thread resumed\n");
	k_event_post(&cg_streamEvent, EVENT_RESUME_EVENT);
	res = k_event_wait(&cg_streamReplyEvent,
			   EVENT_RESUMED_EVENT | EVENT_PAUSED_EVENT, false,
			   K_FOREVER);
	if ((res & EVENT_PAUSED_EVENT) != 0)
	{
		k_event_clear(&cg_streamReplyEvent, EVENT_PAUSED_EVENT);
		LOG_ERR("Event thread failed to resume\n");
		context->evtQueue->pause();
		report_runtime_error(kEventThread, CG_RESUME_FAILURE,
				     CG_UNIDENTIFIED_NODE, 0);
		if (context->scheduler_length > 0)
		{
			k_event_post(&cg_streamEvent, STREAM_PAUSE_EVENT);
			res = k_event_wait(&cg_streamReplyEvent, STREAM_PAUSED_EVENT, false,
					   K_FOREVER);
			if ((res & STREAM_PAUSED_EVENT) != 0)
			{
				k_event_clear(&cg_streamReplyEvent, STREAM_PAUSED_EVENT);
			}
		}
		return false;
	}
	if ((res & EVENT_RESUMED_EVENT) != 0)
	{
		k_event_clear(&cg_streamReplyEvent, EVENT_RESUMED_EVENT);
	}
	k_event_clear(&cg_streamReplyEvent, STREAM_PAUSED_EVENT | EVENT_PAUSED_EVENT);
	LOG_DBG("Stream scheduler and event queue resumed\n");
	return true;
}

int stream_init_memory()
{
	LOG_DBG("Initializing stream memory\n");
	/* Init events */
	k_event_init(&cg_streamEvent);
	k_event_init(&cg_streamReplyEvent);

	/* Init memory slabs */
	int err = k_mem_slab_init(&cg_eventPool, list_slab_area, LIST_ELEMENT_SIZE,
							  CONFIG_CMSISSTREAM_NB_MAX_EVENTS);
	if (err != 0)
	{
		LOG_ERR("Failed to init event pool slab\n");
		return (err);
	}

	err = k_mem_slab_init(&cg_bufPool, buf_slab_area, BUF_ELEMENT_SIZE,
						  CONFIG_CMSISSTREAM_NB_MAX_BUFS);
	if (err != 0)
	{
		LOG_ERR("Failed to init buf pool slab\n");
		return (err);
	}

	err = k_mem_slab_init(&cg_mutexPool, mutex_slab_area, MUTEX_ELEMENT_SIZE,
						  CONFIG_CMSISSTREAM_NB_MAX_BUFS);
	if (err != 0)
	{
		LOG_ERR("Failed to init mutex pool slab\n");
		return (err);
	}

	LOG_DBG("Stream memory initialized\n");

	return (0);
}

bool stream_start_threads(stream_execution_context_t *context)
{
	if ((context == nullptr) || (context->evtQueue == nullptr))
	{
		LOG_ERR("Can't start stream threads with invalid context\n");
		return false;
	}

	set_current_context(context);

	tid_event = k_thread_create(
		&event_thread, event_thread_stack, K_THREAD_STACK_SIZEOF(event_thread_stack),
		event_thread_function, NULL, NULL, NULL, CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY, K_FP_REGS, K_NO_WAIT);
	if (tid_event == nullptr)
	{
		LOG_ERR("Failed to start event thread\n");
		set_current_context(nullptr);
		return false;
	}

	k_thread_name_set(&event_thread, "event_thread");

	tid_stream =
		k_thread_create(&stream_thread, stream_thread_stack,
						K_THREAD_STACK_SIZEOF(stream_thread_stack), stream_thread_function,
						NULL, NULL, NULL, CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY, K_FP_REGS, K_NO_WAIT);
	if (tid_stream == nullptr)
	{
		LOG_ERR("Failed to start stream thread\n");
		cleanup_started_event_thread();
		return false;
	}

	k_thread_name_set(&stream_thread, "stream_thread");

	LOG_DBG("Stream runtime threads started\n");
	return true;
}

void stream_wait_for_threads_end()
{
	if (tid_stream != nullptr)
	{
		k_thread_join(&stream_thread, K_FOREVER);
		tid_stream = nullptr;
	}
	if (tid_event != nullptr)
	{
		k_thread_join(&event_thread, K_FOREVER);
		tid_event = nullptr;
	}
}

void stream_free_memory()
{
}

EventQueue *stream_new_event_queue()
{
	MyQueue *queue = new (std::nothrow) MyQueue(CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY,
												CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY,
												CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY);
	return (static_cast<EventQueue *>(queue));
}
