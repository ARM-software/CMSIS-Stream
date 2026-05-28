/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        stream_event_queue.hpp
 * Description:  POSIX event queue implementation for CMSIS-Stream
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2023-2026 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <condition_variable>
#include <mutex>
#include <utility>

#include "EventQueue.hpp"
#include "StreamNode.hpp"
#include "cg_enums.h"
#include "stream_platform_config.hpp"

#define POSIX_QUEUE_MAX_ELEMS CMSISSTREAM_EVENT_QUEUE_LENGTH

class PosixEventQueue : public arm_cmsis_stream::EventQueue {
  public:
    PosixEventQueue(ThreadPriority low, ThreadPriority normal, ThreadPriority high);
    ~PosixEventQueue();

    bool push(arm_cmsis_stream::Message &&event) final;
    bool isEmpty() final;
    void clear() final;
    void execute() final;
    void end() noexcept final;
    void pause() noexcept final;

  private:
    void waitEvent();
    void notifyQueue() noexcept;

    CG_MUTEX queue_mutex;
    std::condition_variable cv_;
    bool event_ = false;
    std::mutex cv_mutex_;

  protected:
    constexpr static uint32_t nb_priorities = 3;
    arm_cmsis_stream::Message *queue[nb_priorities];
    int read[nb_priorities];
    int write[nb_priorities];
    uint32_t nb_elems[nb_priorities];
    ThreadPriority priorities[nb_priorities];
};
