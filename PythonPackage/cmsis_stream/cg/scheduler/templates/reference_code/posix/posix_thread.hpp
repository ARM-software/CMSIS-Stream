/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        posix_thread.hpp
 * Description:  Example implementation of a thread for CMSIS-Stream events
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
 *
 * Copyright (C) 2023-2025 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once


#include <functional>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "cg_enums.h"
#include "config_events.h"
#include "custom.hpp"
#include "StreamNode.hpp"
#include "EventQueue.hpp"



enum class ThreadPriority
    {
        Low,
        Normal,
        High,
        RealTime
};

    class PlatformThread
    {
    public:
        PlatformThread() {};
        ~PlatformThread() {};

        virtual void start() = 0;
        virtual void setPriority(ThreadPriority priority) = 0;
        virtual void sleep() = 0;
        virtual void wakeup() = 0;
        virtual void join() = 0;
        virtual void waitUntilStarted() = 0;
    };

#if defined(CG_EVENTS_MULTI_THREAD)
    extern PlatformThread *cg_eventThread;
#endif

class PosixThread : public PlatformThread
{
public:
    PosixThread(std::function<void()> func);
    ~PosixThread();

    void start() final;
    void setPriority(ThreadPriority priority) final;
    void sleep() final;
    void wakeup() final;
    void join() final;
    void waitUntilStarted() final;

private:
    std::function<void()> userFunc;
    std::thread thread;
    std::atomic<bool> shouldSleep{false};
    std::mutex sleepMutex;
    std::condition_variable sleepCV;

    std::mutex startMutex;
    std::condition_variable startCV;
    std::atomic<bool> hasStarted = false;

    void run();
    void platformSetPriority(ThreadPriority priority);
};

