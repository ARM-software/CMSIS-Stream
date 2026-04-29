#include "stream_runtime_config.hpp"
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


#ifndef CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE
#define CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE 4096
#endif 

#ifndef CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE
#define CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE 4096
#endif

#ifndef CONFIG_CMSISSTREAM_NB_MAX_EVENTS
#define CONFIG_CMSISSTREAM_NB_MAX_EVENTS 16
#endif

#ifndef CONFIG_CMSISSTREAM_NB_MAX_BUFS
#define CONFIG_CMSISSTREAM_NB_MAX_BUFS 16
#endif

#ifndef CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY
#define CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY osPriorityHigh
#endif

#ifndef CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY
#define CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY osPriorityRealtime
#endif

#ifndef CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY
#define CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY osPriorityNormal
#endif

#ifndef CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY
#define CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY osPriorityLow
#endif

#ifndef CONFIG_CMSISSTREAM_SHARED_OVERHEAD
#define CONFIG_CMSISSTREAM_SHARED_OVERHEAD 16
#endif

#ifndef CONFIG_CMSISSTREAM_POOL_SECTION
#define CONFIG_CMSISSTREAM_POOL_SECTION ".bss.evt_pool"
#endif


osEventFlagsId_t cg_streamEvent;
osEventFlagsId_t cg_streamReplyEvent;

using namespace arm_cmsis_stream;

// Define memory pool sizes
#define LIST_ELEMENT_SIZE (sizeof(ListValue) + sizeof(std::shared_ptr<ListValue>) + CONFIG_CMSISSTREAM_SHARED_OVERHEAD)
#define LIST_SIZE (CONFIG_CMSISSTREAM_NB_MAX_EVENTS * LIST_ELEMENT_SIZE)

#define BUF_ELEMENT_SIZE (sizeof(Tensor<double>) + sizeof(std::shared_ptr<Tensor<double>>) + CONFIG_CMSISSTREAM_SHARED_OVERHEAD)
#define BUF_SIZE (CONFIG_CMSISSTREAM_NB_MAX_BUFS * BUF_ELEMENT_SIZE)

#define MUTEX_ELEMENT_SIZE (sizeof(CG_MUTEX) + sizeof(std::shared_ptr<CG_MUTEX>) + CONFIG_CMSISSTREAM_SHARED_OVERHEAD)
#define MUTEX_SIZE (CONFIG_CMSISSTREAM_NB_MAX_BUFS * MUTEX_ELEMENT_SIZE)

__ALIGNED(8) __attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t list_slab_area[LIST_SIZE];

__ALIGNED(8) __attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t buf_slab_area[BUF_SIZE];

__ALIGNED(8) __attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t mutex_slab_area[MUTEX_SIZE];

osMemoryPoolId_t cg_eventPool;
osMemoryPoolId_t cg_bufPool;
osMemoryPoolId_t cg_mutexPool;

static osThreadId_t tid_stream = nullptr;
static osThreadId_t tid_event = nullptr;

static const osThreadAttr_t stream_thread_attr = {
	    .attr_bits = osThreadJoinable,
        .stack_size = CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE,
        .priority = CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY
    };

static const osThreadAttr_t event_thread_attr = {
	    .attr_bits = osThreadJoinable,
        .stack_size = CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE,
        .priority = CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY,
    };


using namespace arm_cmsis_stream;

std::atomic<stream_execution_context_t *> current_context = nullptr;

static void event_thread_function(void *arg)
{

	CMSISSTREAM_LOG_DBG("Started event thread\n");
	bool done = false;

	while (!done)
	{
		// Process current queue
		stream_execution_context_t *context = current_context.load();
		if (context == nullptr)
		{
			CMSISSTREAM_LOG_ERR("No stream execution context defined\n");
			return;
		}
		EventQueue *queue = context->evtQueue;
		queue->execute();
		if (queue->mustEnd())
		{
			done = true;
		}
		else // Paused queue
		{
			// If no dataflow nodes, the nodes are paused here when the event thread is pausing
			if (context->scheduler_length == 0)
			{
				if (context->pause_all_nodes)
					context->pause_all_nodes(context);
			}
			osEventFlagsSet(cg_streamReplyEvent, EVENT_PAUSED_EVENT);
			uint32_t res = osEventFlagsWait(cg_streamEvent, EVENT_RESUME_EVENT, osFlagsWaitAny, osWaitForever );
			osEventFlagsSet(cg_streamReplyEvent, EVENT_RESUMED_EVENT);
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
		if (context == nullptr)
		{
			LOG_ERR("No stream execution context defined\n");
			return;
		}
		nb_iter = context->dataflow_scheduler(&error);
		if ((error != CG_SUCCESS) &&
			(error != CG_STOP_SCHEDULER) &&
			(error != CG_PAUSED_SCHEDULER))
		{
			CMSISSTREAM_LOG_ERR("Scheduler error %d\n", error);
		}
		// When there is no dataflow node, the data flow graph
		// is returning immediately with 0 iteration
		// This should be interpreted as a pause
		if ((error == CG_PAUSED_SCHEDULER) ||
			((nb_iter == 0) && (error == CG_STOP_SCHEDULER)))
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

			
			uint32_t res = osEventFlagsWait(cg_streamEvent, STREAM_RESUME_EVENT, osFlagsWaitAny, osWaitForever);
			if ((res & STREAM_RESUME_EVENT) != 0)
			{
				// The context may have changed after a resume event
				context = current_context.load();
				// Clear FIFOs here. In case of memory overlay between different graphs, the
				// data in FIFOs could be corrupted when resuming another graph
				context->reset_fifos(1);
				if (context->resume_all_nodes)
					context->resume_all_nodes(context);
				osEventFlagsSet(cg_streamReplyEvent, STREAM_RESUMED_EVENT);

				CMSISSTREAM_LOG_DBG("Scheduler resumed\n");
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

void stream_resume_scheduler(stream_execution_context_t *context)
{
	CMSISSTREAM_LOG_DBG("Resuming stream scheduler and event queue\n");
	current_context.store(context);
	current_context.load()->evtQueue->resume();
	osEventFlagsSet(cg_streamEvent, STREAM_RESUME_EVENT);
	uint32_t res = osEventFlagsWait(cg_streamReplyEvent, STREAM_RESUMED_EVENT, osFlagsWaitAny, osWaitForever);
	if ((res & STREAM_RESUMED_EVENT) != 0)
	{
		// No need to clear the event flag in CMSIS-RTOS2
	}
	CMSISSTREAM_LOG_DBG("Stream thread resumed\n");
	osEventFlagsSet(cg_streamEvent, EVENT_RESUME_EVENT);
	res = osEventFlagsWait(cg_streamReplyEvent, EVENT_RESUMED_EVENT, osFlagsWaitAny, osWaitForever);
	if ((res & EVENT_RESUMED_EVENT) != 0)
	{
		// No need to clear the event flag in CMSIS-RTOS2
	}
	CMSISSTREAM_LOG_DBG("Stream scheduler and event queue resumed\n");
}

int stream_init_memory()
{
	CMSISSTREAM_LOG_DBG("Initializing stream memory\n");
	/* Init events */
	cg_streamEvent = osEventFlagsNew(NULL);
	cg_streamReplyEvent = osEventFlagsNew(NULL);

	/* Init memory slabs */
    cg_eventPool = osMemoryPoolNew(CONFIG_CMSISSTREAM_NB_MAX_EVENTS, LIST_ELEMENT_SIZE, NULL);

	
	if (cg_eventPool == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to init event pool slab\n");
		return (-1);
	}

	cg_bufPool = osMemoryPoolNew(CONFIG_CMSISSTREAM_NB_MAX_BUFS, BUF_ELEMENT_SIZE, NULL);
	if (cg_bufPool == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to init buf pool slab\n");
		return (-1);
	}

	cg_mutexPool = osMemoryPoolNew(CONFIG_CMSISSTREAM_NB_MAX_BUFS, MUTEX_ELEMENT_SIZE, NULL);
	if (cg_mutexPool == nullptr)
	{
		CMSISSTREAM_LOG_ERR("Failed to init mutex pool slab\n");
		return (-1);
	}

	CMSISSTREAM_LOG_DBG("Stream memory initialized\n");

	return (0);
}

void stream_start_threads(stream_execution_context_t *context)
{

	current_context.store(context);

    tid_event = osThreadNew(event_thread_function, NULL, &event_thread_attr);

	tid_stream = osThreadNew(stream_thread_function, NULL, &stream_thread_attr);

	CMSISSTREAM_LOG_DBG("Stream runtime threads started\n");
}

void stream_wait_for_threads_end()
{
	osThreadJoin(tid_stream);
	osThreadJoin(tid_event);
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
	MyQueue *queue = new (std::nothrow) MyQueue(CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY,
												CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY,
												CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY);
	return (static_cast<EventQueue *>(queue));
}
