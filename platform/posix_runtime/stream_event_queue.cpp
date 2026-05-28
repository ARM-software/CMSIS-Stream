/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        stream_event_queue.cpp
 * Description:  POSIX event queue implementation for CMSIS-Stream
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2023-2026 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stream_event_queue.hpp"

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>
#include <variant>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#include <sched.h>
#endif

using namespace arm_cmsis_stream;

void *EventQueue::handlerData = nullptr;
EventQueue::AppHandler EventQueue::handler = nullptr;
std::atomic<bool> EventQueue::handlerReady_ = false;

void stream_set_current_thread_priority(ThreadPriority priority)
{
#ifdef _WIN32
    int winPriority = THREAD_PRIORITY_NORMAL;
    switch (priority) {
    case ThreadPriority::Low:
        winPriority = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case ThreadPriority::High:
        winPriority = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case ThreadPriority::RealTime:
        winPriority = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    case ThreadPriority::Normal:
    default:
        break;
    }
    SetThreadPriority(GetCurrentThread(), winPriority);
#elif defined(__linux__) || defined(__APPLE__)
    int policy = SCHED_OTHER;
    sched_param sch_params{};

    switch (priority) {
    case ThreadPriority::High:
        policy = SCHED_RR;
        sch_params.sched_priority = 50;
        break;
    case ThreadPriority::RealTime:
        policy = SCHED_FIFO;
        sch_params.sched_priority = 80;
        break;
    case ThreadPriority::Low:
    case ThreadPriority::Normal:
    default:
        sch_params.sched_priority = 0;
        break;
    }

    (void)pthread_setschedparam(pthread_self(), policy, &sch_params);
#else
    (void)priority;
#endif
}

PosixEventQueue::PosixEventQueue(ThreadPriority low, ThreadPriority normal, ThreadPriority high)
    : arm_cmsis_stream::EventQueue()
{
    priorities[0] = low;
    priorities[1] = normal;
    priorities[2] = high;
    for (uint32_t p = 0; p < nb_priorities; p++) {
        queue[p] = new (std::nothrow) Message[POSIX_QUEUE_MAX_ELEMS];
        read[p] = 0;
        write[p] = 0;
        nb_elems[p] = 0;
    }
}

PosixEventQueue::~PosixEventQueue()
{
    for (uint32_t p = 0; p < nb_priorities; p++) {
        delete[] queue[p];
    }
}

bool PosixEventQueue::push(arm_cmsis_stream::Message &&event)
{
    bool ok = false;
    if (this->mustPause() || this->mustEnd()) {
        return false;
    }

    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    if (!CG_MUTEX_HAS_ERROR(error)) {
        uint32_t p = event.event.priority;
        if (p >= nb_priorities) {
            p = nb_priorities - 1;
        }
        if (nb_elems[p] < POSIX_QUEUE_MAX_ELEMS) {
            event.timestamp = CG_GET_TIME_STAMP();
            queue[p][write[p]++] = std::move(event);
            if (write[p] == POSIX_QUEUE_MAX_ELEMS) {
                write[p] = 0;
            }

            nb_elems[p]++;
            ok = true;
        } else {
            LOG_ERR("Event queue overflow for priority %u\n", p);
            this->setError(CG_EVENT_QUEUE_FULL);
        }
    } else {
        this->setError(CG_OS_ERROR, CG_UNIDENTIFIED_NODE, static_cast<int32_t>(error));
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);

    notifyQueue();
    return ok;
}

bool PosixEventQueue::isEmpty()
{
    bool r = true;
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    for (uint32_t p = 0; p < nb_priorities; p++) {
        if (nb_elems[p] != 0) {
            r = false;
            break;
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
    return r;
}

void PosixEventQueue::clear()
{
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    if (!CG_MUTEX_HAS_ERROR(error)) {
        for (uint32_t p = 0; p < nb_priorities; p++) {
            while (nb_elems[p] != 0) {
                Message msg = std::move(queue[p][read[p]++]);
                if (read[p] == POSIX_QUEUE_MAX_ELEMS) {
                    read[p] = 0;
                }
                nb_elems[p]--;
                msg = Message();
            }
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
}

void PosixEventQueue::end() noexcept
{
    mustEnd_.store(true);
    notifyQueue();
}

void PosixEventQueue::pause() noexcept
{
    mustPause_.store(true);
    notifyQueue();
}

void PosixEventQueue::waitEvent()
{
    std::unique_lock<std::mutex> lock(cv_mutex_);
    cv_.wait(lock, [this] { return event_; });
    event_ = false;
}

void PosixEventQueue::notifyQueue() noexcept
{
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        event_ = true;
    }
    cv_.notify_one();
}

void PosixEventQueue::execute()
{
    CG_MUTEX_ERROR_TYPE error;
    while ((!this->mustEnd()) && (!this->mustPause())) {
        while ((!this->mustEnd()) && (!this->mustPause()) && (!isEmpty())) {
            Message msg;
            bool messageWasReceived = false;
            CG_ENTER_CRITICAL_SECTION(queue_mutex, error);

            if (!CG_MUTEX_HAS_ERROR(error)) {
                for (int32_t p = nb_priorities - 1; p >= 0; p--) {
                    if (nb_elems[p] != 0) {
                        msg = std::move(queue[p][read[p]++]);
                        if (read[p] == POSIX_QUEUE_MAX_ELEMS) {
                            read[p] = 0;
                        }

                        nb_elems[p]--;
                        messageWasReceived = true;
                        break;
                    }
                }
            } else {
                this->setError(CG_OS_ERROR, CG_UNIDENTIFIED_NODE, static_cast<int32_t>(error));
            }
            CG_EXIT_CRITICAL_SECTION(queue_mutex, error);

            if (messageWasReceived) {
                bool eventExpired = false;
                if (msg.event.ttl != 0) {
                    uint32_t limitMs = msg.timestamp + msg.event.ttl;
                    uint32_t nowMs = CG_GET_TIME_STAMP();
                    if (nowMs > limitMs) {
                        eventExpired = true;
                    }
                }
                if (!eventExpired) {
                    uint32_t p = msg.event.priority;
                    if (p >= nb_priorities) {
                        p = nb_priorities - 1;
                    }
                    stream_set_current_thread_priority(priorities[p]);

                    if (std::holds_alternative<LocalDestination>(msg.destination)) {
                        LocalDestination &local = std::get<LocalDestination>(msg.destination);
                        cg_status status = local.dst->processEvent(local.dstPort, std::move(msg.event));
                        if (status != CG_SUCCESS) {
                            this->setError(status, local.dst->nodeID());
                        }
                    } else if (std::holds_alternative<DistantDestination>(msg.destination)) {
                        DistantDestination &dist = std::get<DistantDestination>(msg.destination);
                        if (!this->callAsyncHandler(dist.src_node_id, std::move(msg.event))) {
                            this->setError(CG_EVENT_QUEUE_FULL, dist.src_node_id);
                        }
                    }
                    stream_set_current_thread_priority(priorities[nb_priorities - 1]);
                }
            }
        }
        if (this->mustEnd() || this->mustPause()) {
            return;
        }
        waitEvent();
    }
}
