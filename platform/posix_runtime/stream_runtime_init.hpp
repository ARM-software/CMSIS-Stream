#pragma once

#include <condition_variable>
#include <cstdint>
#include <mutex>

#include "EventQueue.hpp"
#include "stream_platform_config.hpp"

constexpr uint32_t STREAM_WAIT_FOREVER = UINT32_MAX;

class stream_event_flags {
  public:
    void post(uint32_t flags);
    void clear(uint32_t flags);
    uint32_t wait(uint32_t flags, bool clear, uint32_t timeout_ms = STREAM_WAIT_FOREVER);

  private:
    std::mutex mutex_;
    std::condition_variable cv_;
    uint32_t flags_ = 0;
};

extern stream_event_flags cg_streamEvent;
extern stream_event_flags cg_streamReplyEvent;

struct stream_execution_context_t_;

typedef uint32_t (*stream_scheduler)(int *error);
typedef void (*stream_reset_fifos)(int all);
typedef void (*pause_nodes)(const struct stream_execution_context_t_ *);
typedef void (*resume_nodes)(const struct stream_execution_context_t_ *);
typedef void *(*get_scheduler_node)(int32_t nodeID);

typedef struct stream_execution_context_t_ {
    stream_scheduler dataflow_scheduler;
    stream_reset_fifos reset_fifos;
    pause_nodes pause_all_nodes;
    resume_nodes resume_all_nodes;
    get_scheduler_node get_node_by_id;
    arm_cmsis_stream::EventQueue *evtQueue;
    uint32_t nb_identified_nodes;
    uint32_t scheduler_length;
} stream_execution_context_t;

extern void stream_pause_current_scheduler();
extern bool stream_resume_scheduler(stream_execution_context_t *context);
extern int stream_init_memory();
extern bool stream_start_threads(stream_execution_context_t *context);
/*
 * Request runtime threads to stop.
 *
 * The function always wakes paused/runtime waits and joins peer runtime threads
 * before returning.
 *
 * @param callerIsRuntimeThread Set to true when this function is called from a
 * runtime callback or graph node that is running on one of the runtime threads.
 * Peer runtime threads are always joined before this function returns. When the
 * caller is itself a runtime thread, that current thread is detached because a
 * thread cannot join itself.
 */
extern void stream_stop_threads(bool callerIsRuntimeThread = false);
extern void stream_wait_for_threads_end();
extern void stream_free_memory();
extern arm_cmsis_stream::EventQueue *stream_new_event_queue();
