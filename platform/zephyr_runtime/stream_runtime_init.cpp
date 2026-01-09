#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(streamruntime_module);

#include "stream_platform_config.hpp"
#include "EventQueue.hpp"
#include "StreamNode.hpp"

#include "stream_event_queue.hpp"

#include "stream_runtime_init.hpp"


#include <new>

#define MEMORY_POOL_HEAP_SIZE CONFIG_CMSISSTREAM_POOL_SIZE 

__aligned(8)
__attribute__((section(CONFIG_CMSISSTREAM_POOL_SECTION))) static uint8_t sram0_heap_area[MEMORY_POOL_HEAP_SIZE];

static struct k_heap sram0_heap;

struct k_mem_slab cg_eventPool;
struct k_mem_slab cg_bufPool;
struct k_mem_slab cg_mutexPool;



static void *event_pool_buffer;
static void *buf_pool_buffer;
static void *mutex_pool_buffer;

static k_tid_t tid_stream = nullptr;
static k_tid_t tid_event = nullptr;

static struct k_thread stream_thread;
static struct k_thread event_thread;


static K_THREAD_STACK_DEFINE(event_thread_stack, CONFIG_CMSISSTREAM_EVT_THREAD_STACK_SIZE);
static K_THREAD_STACK_DEFINE(stream_thread_stack, CONFIG_CMSISSTREAM_STREAM_THREAD_STACK_SIZE);

using namespace arm_cmsis_stream;

static void event_thread_function(void *evtQueue, void *, void *)
{
	EventQueue *queue =
		reinterpret_cast<EventQueue *>(evtQueue);
	LOG_DBG("Started event thread\n");

	queue->execute();

}

static void stream_thread_function(void *context_, void *, void *)
{
	stream_execution_context_t *context = reinterpret_cast<stream_execution_context_t*>(context_);
	uint32_t nb_iter;
	int error;
	LOG_DBG("Stream thread started\n");

	nb_iter = context->dataflow_scheduler(&error);
	if ((error != CG_SUCCESS) && (error != CG_STOP_SCHEDULER)) {
		LOG_ERR("Scheduler error %d\n", error);
	}
	LOG_DBG("Scheduler done after %d iterations\n", nb_iter);
}

int stream_init_memory()
{
	/* Init sram0 heap */
	k_heap_init(&sram0_heap, sram0_heap_area, MEMORY_POOL_HEAP_SIZE);

	/* Init memory slabs */
	event_pool_buffer = nullptr;
	buf_pool_buffer = nullptr;
	mutex_pool_buffer = nullptr;

	event_pool_buffer =
		k_heap_alloc(&sram0_heap, CONFIG_CMSISSTREAM_NB_MAX_EVENTS * (sizeof(ListValue) + 16), K_NO_WAIT);
	int err = k_mem_slab_init(&cg_eventPool, event_pool_buffer, sizeof(ListValue) + 16,
				  CONFIG_CMSISSTREAM_NB_MAX_EVENTS);
	if (err != 0) {
		LOG_ERR("Failed to init event pool slab\n");
		return (err);
	}

	buf_pool_buffer =
		k_heap_alloc(&sram0_heap, CONFIG_CMSISSTREAM_NB_MAX_BUFS * (sizeof(Tensor<double>) + 16), K_NO_WAIT);
	err = k_mem_slab_init(&cg_bufPool, buf_pool_buffer, sizeof(Tensor<double>) + 16,
			      CONFIG_CMSISSTREAM_NB_MAX_BUFS);
	if (err != 0) {
		LOG_ERR("Failed to init buf pool slab\n");
		return (err);
	}

	mutex_pool_buffer =
		k_heap_alloc(&sram0_heap, CONFIG_CMSISSTREAM_NB_MAX_BUFS * (sizeof(CG_MUTEX) + 16), K_NO_WAIT);
	err = k_mem_slab_init(&cg_mutexPool, mutex_pool_buffer, sizeof(CG_MUTEX) + 16, CONFIG_CMSISSTREAM_NB_MAX_BUFS);
	if (err != 0) {
		LOG_ERR("Failed to init mutex pool slab\n");
		return (err);
	}

	
	

	LOG_DBG("Stream memory initialized\n");

	return (0);
}

void stream_start_threads(stream_execution_context_t *context)
{

	tid_event = k_thread_create(
		&event_thread, event_thread_stack, K_THREAD_STACK_SIZEOF(event_thread_stack),
		event_thread_function, context->evtQueue,NULL, NULL, CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY, K_FP_REGS, K_NO_WAIT);

	k_thread_name_set(&event_thread, "event_thread");

	tid_stream =
		k_thread_create(&stream_thread, stream_thread_stack,
				K_THREAD_STACK_SIZEOF(stream_thread_stack), stream_thread_function,
				 (void*)context,NULL, NULL,CONFIG_CMSISSTREAM_STREAM_THREAD_PRIORITY, K_FP_REGS, K_NO_WAIT);

	k_thread_name_set(&stream_thread, "stream_thread");

	LOG_DBG("Stream thread started\n");
}

void stream_wait_for_threads_end()
{
	k_thread_join(&stream_thread, K_FOREVER);
	k_thread_join(&event_thread, K_FOREVER);
}

void stream_free_memory()
{
   
   k_heap_free(&sram0_heap, event_pool_buffer);
   k_heap_free(&sram0_heap, buf_pool_buffer);
   k_heap_free(&sram0_heap, mutex_pool_buffer);

}

EventQueue *stream_new_event_queue()
{
	MyQueue *queue = new (std::nothrow) MyQueue(CONFIG_CMSISSTREAM_EVT_LOW_PRIORITY,
										   CONFIG_CMSISSTREAM_EVT_NORMAL_PRIORITY,
										   CONFIG_CMSISSTREAM_EVT_HIGH_PRIORITY);
	return (static_cast<EventQueue *>(queue));

}

