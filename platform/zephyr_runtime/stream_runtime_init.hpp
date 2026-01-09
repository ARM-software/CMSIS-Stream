#pragma once

#include <zephyr/kernel.h>

#include "stream_platform_config.hpp"
#include "EventQueue.hpp"


typedef uint32_t (*stream_scheduler)(int *error);

/**
 * @brief Structure to hold the stream execution context
 * This structure encapsulates the scheduler function,
 * its parameters, and the event queue used by the CMSIS Stream runtime.
 */
typedef struct {
   stream_scheduler dataflow_scheduler;
   arm_cmsis_stream::EventQueue *evtQueue;
} stream_execution_context_t;

/**
 * @brief Initialize the CMSIS Stream runtime memory and structures
 * @return 0 on success, negative error code on failure
 * 
 * This function initializes the memory pools and event queue required
 * for the CMSIS Stream runtime in a Zephyr environment.
 */
extern int stream_init_memory();

/**
 * @brief Start the CMSIS Stream runtime threads
 * This function creates and starts the necessary threads for
 * handling CMSIS Stream events and processing.
 */
extern void stream_start_threads(stream_execution_context_t *context);

/**
 * @brief Wait for the CMSIS Stream threads to finish
 * This function blocks until the stream and event threads
 * have completed their execution.
 */
extern void stream_wait_for_threads_end();

/**
 * @brief Free the CMSIS Stream runtime memory
 * This function releases the memory allocated for the CMSIS Stream
 * runtime, including the event queue.
 */
extern void stream_free_memory();

/**
 * @brief Create a new CMSIS Stream Event Queue
 * @return Pointer to the newly created Event Queue, or nullptr on failure
 * This function also set this event queue as the current queue for the
 * event thread. This function should be called only when the event thread
 * is not running.
 */
extern arm_cmsis_stream::EventQueue *stream_new_event_queue();


