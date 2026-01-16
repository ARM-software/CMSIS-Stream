#pragma once

#include <zephyr/kernel.h>

#include "stream_platform_config.hpp"
#include "EventQueue.hpp"

extern struct k_event cg_streamEvent;
extern struct k_event cg_streamReplyEvent;

struct stream_execution_context_t_;

typedef uint32_t (*stream_scheduler)(int *error);
typedef void (*stream_reset_fifos)(int all);
typedef void (*pause_nodes)(const struct stream_execution_context_t_*);
typedef void (*resume_nodes)(const struct stream_execution_context_t_*);
typedef void* (*get_scheduler_node)(int32_t nodeID);

/**
 * @brief Structure to hold the stream execution context
 * This structure encapsulates the scheduler function,
 * its parameters, and the event queue used by the CMSIS Stream runtime.
 */
typedef struct stream_execution_context_t_ {
   stream_scheduler dataflow_scheduler;
   stream_reset_fifos reset_fifos;
   pause_nodes pause_all_nodes;
   resume_nodes resume_all_nodes;
   get_scheduler_node get_node_by_id;
   arm_cmsis_stream::EventQueue *evtQueue;
   uint32_t nb_identified_nodes;
} stream_execution_context_t;

/**
 * @brief Pause the current CMSIS stream graph (dataflow and events)
 * This function signals the stream and event threads to pause
 * their processing. It waits until both threads have acknowledged
 */
extern void stream_pause_current_scheduler();

/**
 * @brief Resume the CMSIS stream graph (dataflow and events)
 * @param context Pointer to the stream execution context to resume
 * This function signals the stream and event threads to resume
 * their processing after a pause.
 * 
 * If the context parameter is different from the current context,
 * it updates the current context before resuming.
 * As consequence it can be used to context switch between different graphs.
 */
extern void stream_resume_scheduler(stream_execution_context_t *context);

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


