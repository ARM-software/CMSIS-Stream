#include "stream_platform_config.hpp"

#include "EventQueue.hpp"
#include "StreamNode.hpp"
#include "cg_enums.h"
#include "stream_event_queue.hpp"
#include "stream_rtos_events.h"
#include "stream_runtime_init.hpp"

#include <atomic>
#include <chrono>
#include <memory_resource>
#include <new>
#include <thread>

using namespace arm_cmsis_stream;

std::pmr::synchronized_pool_resource pool;

stream_event_flags cg_streamEvent;
stream_event_flags cg_streamReplyEvent;

extern void stream_set_current_thread_priority(ThreadPriority priority);

static std::thread *stream_thread = nullptr;
static std::thread *event_thread = nullptr;
static std::atomic<bool> stream_thread_started = false;
static std::atomic<bool> event_thread_started = false;

std::atomic<stream_execution_context_t *> current_context = nullptr;

void stream_event_flags::post(uint32_t flags)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        flags_ |= flags;
    }
    cv_.notify_all();
}

void stream_event_flags::clear(uint32_t flags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    flags_ &= ~flags;
}

uint32_t stream_event_flags::wait(uint32_t flags, bool clear_flags, uint32_t timeout_ms)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto ready = [this, flags] { return (flags_ & flags) != 0; };

    if (timeout_ms == 0) {
        if (!ready()) {
            return 0;
        }
    } else if (timeout_ms == STREAM_WAIT_FOREVER) {
        cv_.wait(lock, ready);
    } else {
        if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), ready)) {
            return 0;
        }
    }

    uint32_t res = flags_ & flags;
    if (clear_flags) {
        flags_ &= ~res;
    }
    return res;
}

static void set_current_context(stream_execution_context_t *context)
{
    current_context.store(context);
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
    EventQueue::callSyncHandler(CG_UNIDENTIFIED_NODE, Event(kStopGraph, kHighPriority));
}

static void wait_stream_resume(stream_execution_context_t *context)
{
    while (true) {
        uint32_t res = cg_streamEvent.wait(STREAM_RESUME_EVENT, true);
        if ((res & STREAM_RESUME_EVENT) == 0) {
            continue;
        }

        context = current_context.load();
        context->reset_fifos(1);
        if (context->resume_all_nodes) {
            context->resume_all_nodes(context);
        }
        cg_streamReplyEvent.post(STREAM_RESUMED_EVENT);

        CMSISSTREAM_LOG_DBG("Scheduler resumed\n");
        return;
    }
}

static void wait_event_thread_paused()
{
    (void)cg_streamReplyEvent.wait(EVENT_PAUSED_EVENT, true);
}

static void wait_event_resume()
{
    while (true) {
        uint32_t res = cg_streamEvent.wait(EVENT_RESUME_EVENT, true);
        if ((res & EVENT_RESUME_EVENT) == 0) {
            continue;
        }
        cg_streamReplyEvent.post(EVENT_RESUMED_EVENT);
        return;
    }
}

static void pause_stream_thread_on_error(stream_execution_context_t *context, cg_status status)
{
    CMSISSTREAM_LOG_DBG("Try to pause event queue after stream error\n");
    context->evtQueue->pause();

    if (context->scheduler_length > 0) {
        if (context->pause_all_nodes) {
            context->pause_all_nodes(context);
        }
    }

    wait_event_thread_paused();
    report_runtime_error(kStreamThread, status, CG_UNIDENTIFIED_NODE, 0);
    cg_streamReplyEvent.post(STREAM_PAUSED_EVENT);
    wait_stream_resume(context);
}

static void pause_stream_thread_on_stop_graph(stream_execution_context_t *context)
{
    CMSISSTREAM_LOG_DBG("Try to pause event queue after stop graph\n");
    context->evtQueue->pause();

    if (context->pause_all_nodes) {
        context->pause_all_nodes(context);
    }

    wait_event_thread_paused();
    report_stop_graph();
    cg_streamReplyEvent.post(STREAM_PAUSED_EVENT);
    wait_stream_resume(context);
}

static void pause_peer_stream_thread(stream_execution_context_t *context)
{
    if (context->scheduler_length > 0) {
        CMSISSTREAM_LOG_DBG("Try to pause stream scheduler after event error\n");
        cg_streamEvent.post(STREAM_PAUSE_EVENT);
        (void)cg_streamReplyEvent.wait(STREAM_PAUSED_EVENT, true);
    }
}

static void event_thread_function()
{
    stream_set_current_thread_priority(CMSISSTREAM_EVT_HIGH_PRIORITY);
    CMSISSTREAM_LOG_DBG("Started event thread\n");
    bool done = false;

    while (!done) {
        stream_execution_context_t *context = current_context.load();
        EventQueue *queue = context->evtQueue;
        queue->execute();
        if (queue->mustEnd()) {
            done = true;
        } else {
            cg_status event_error;
            int32_t event_error_node;
            int32_t event_error_info;
            bool has_error = queue->consumeError(event_error, event_error_node,
                                                 event_error_info);
            if (has_error) {
                pause_peer_stream_thread(context);
            }

            if (context->scheduler_length == 0) {
                if (context->pause_all_nodes) {
                    context->pause_all_nodes(context);
                }
            }
            if (has_error) {
                report_runtime_error(kEventThread, event_error,
                                     event_error_node, event_error_info);
            }
            cg_streamReplyEvent.post(EVENT_PAUSED_EVENT);
            wait_event_resume();
        }
    }
}

static void stream_thread_function()
{
    stream_set_current_thread_priority(CMSISSTREAM_STREAM_THREAD_PRIORITY);

    uint32_t nb_iter = 0;
    int error = CG_SUCCESS;
    bool done = false;
    CMSISSTREAM_LOG_DBG("Stream thread started\n");

    while (!done) {
        stream_execution_context_t *context = current_context.load();
        nb_iter = context->dataflow_scheduler(&error);
        if (is_runtime_scheduler_error(error)) {
            CMSISSTREAM_LOG_ERR("Scheduler error %d\n", error);
            pause_stream_thread_on_error(context, static_cast<cg_status>(error));
            continue;
        }
        if ((context->scheduler_length > 0) && (error == CG_STOP_SCHEDULER)) {
            CMSISSTREAM_LOG_DBG("Scheduler requested stop graph\n");
            pause_stream_thread_on_stop_graph(context);
            continue;
        }
        if ((error == CG_PAUSED_SCHEDULER) ||
            ((context->scheduler_length == 0) && (error == CG_STOP_SCHEDULER))) {
            if (context->scheduler_length > 0) {
                if (context->pause_all_nodes) {
                    context->pause_all_nodes(context);
                }
                cg_streamReplyEvent.post(STREAM_PAUSED_EVENT);
            }

            wait_stream_resume(context);
        } else {
            done = true;
        }
    }
    CMSISSTREAM_LOG_DBG("Scheduler done after %u iterations\n", nb_iter);
}

void stream_pause_current_scheduler()
{
    CMSISSTREAM_LOG_DBG("Try to pause event queue\n");
    current_context.load()->evtQueue->pause();

    if (current_context.load()->scheduler_length > 0) {
        CMSISSTREAM_LOG_DBG("Try to pause stream scheduler\n");
        cg_streamEvent.post(STREAM_PAUSE_EVENT);
        (void)cg_streamReplyEvent.wait(STREAM_PAUSED_EVENT, true);
    }

    (void)cg_streamReplyEvent.wait(EVENT_PAUSED_EVENT, true);
    current_context.load()->evtQueue->clear();

    CMSISSTREAM_LOG_DBG("Stream scheduler and event queue paused\n");
}

bool stream_resume_scheduler(stream_execution_context_t *context)
{
    CMSISSTREAM_LOG_DBG("Resuming stream scheduler and event queue\n");
    if ((context == nullptr) || (context->evtQueue == nullptr)) {
        CMSISSTREAM_LOG_ERR("Can't resume stream scheduler with invalid context\n");
        return false;
    }
    set_current_context(context);
    context->evtQueue->resume();
    cg_streamEvent.post(STREAM_RESUME_EVENT);
    uint32_t res = cg_streamReplyEvent.wait(STREAM_RESUMED_EVENT | STREAM_PAUSED_EVENT, true);
    if ((res & STREAM_PAUSED_EVENT) != 0) {
        CMSISSTREAM_LOG_ERR("Stream thread failed to resume\n");
        context->evtQueue->pause();
        report_runtime_error(kStreamThread, CG_RESUME_FAILURE, CG_UNIDENTIFIED_NODE, 0);
        return false;
    }
    CMSISSTREAM_LOG_DBG("Stream thread resumed\n");
    cg_streamEvent.post(EVENT_RESUME_EVENT);
    res = cg_streamReplyEvent.wait(EVENT_RESUMED_EVENT | EVENT_PAUSED_EVENT, true);
    if ((res & EVENT_PAUSED_EVENT) != 0) {
        CMSISSTREAM_LOG_ERR("Event thread failed to resume\n");
        context->evtQueue->pause();
        report_runtime_error(kEventThread, CG_RESUME_FAILURE, CG_UNIDENTIFIED_NODE, 0);
        if (context->scheduler_length > 0) {
            cg_streamEvent.post(STREAM_PAUSE_EVENT);
            (void)cg_streamReplyEvent.wait(STREAM_PAUSED_EVENT, true);
        }
        return false;
    }
    cg_streamReplyEvent.clear(STREAM_PAUSED_EVENT | EVENT_PAUSED_EVENT);
    CMSISSTREAM_LOG_DBG("Stream scheduler and event queue resumed\n");
    return true;
}

int stream_init_memory()
{
    CMSISSTREAM_LOG_DBG("Initializing stream memory\n");
    CMSISSTREAM_LOG_DBG("Stream memory initialized\n");
    return 0;
}

bool stream_start_threads(stream_execution_context_t *context)
{
    if ((context == nullptr) || (context->evtQueue == nullptr)) {
        CMSISSTREAM_LOG_ERR("Can't start stream threads with invalid context\n");
        return false;
    }

    set_current_context(context);

    try {
        event_thread = new std::thread(event_thread_function);
        event_thread_started.store(true);
        stream_thread = new std::thread(stream_thread_function);
        stream_thread_started.store(true);
    } catch (...) {
        CMSISSTREAM_LOG_ERR("Failed to start stream runtime threads\n");
        if (context->evtQueue != nullptr) {
            context->evtQueue->end();
        }
        if (event_thread != nullptr) {
            if (event_thread->joinable()) {
                event_thread->join();
            }
            delete event_thread;
            event_thread = nullptr;
        }
        set_current_context(nullptr);
        return false;
    }

    CMSISSTREAM_LOG_DBG("Stream runtime threads started\n");
    return true;
}

void stream_wait_for_threads_end()
{
    if (stream_thread_started.load() && (stream_thread != nullptr) && stream_thread->joinable()) {
        stream_thread->join();
        stream_thread_started.store(false);
        delete stream_thread;
        stream_thread = nullptr;
    }
    if (event_thread_started.load() && (event_thread != nullptr) && event_thread->joinable()) {
        event_thread->join();
        event_thread_started.store(false);
        delete event_thread;
        event_thread = nullptr;
    }
}

void stream_free_memory()
{
    pool.release();
}

EventQueue *stream_new_event_queue()
{
    PosixEventQueue *queue = new (std::nothrow) PosixEventQueue(CMSISSTREAM_EVT_LOW_PRIORITY,
                                                               CMSISSTREAM_EVT_NORMAL_PRIORITY,
                                                               CMSISSTREAM_EVT_HIGH_PRIORITY);
    return static_cast<EventQueue *>(queue);
}
