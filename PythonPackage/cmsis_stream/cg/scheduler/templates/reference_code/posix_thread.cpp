/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        posix_thread.cpp
 * Description:  Example implementation of threads for CMSIS-Stream event handling.
 *               Different implementation are possible (RTOS ...)
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

#include "posix_thread.hpp"

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#include <sched.h>
#endif

PosixThread::PosixThread(std::function<void()> func) : PlatformThread(), userFunc(std::move(func)) {}

PosixThread::~PosixThread()
{
    if (thread.joinable())
    {
        thread.join();
    }
}

void PosixThread::start()
{
    thread = std::thread(&PosixThread::run, this);
}

void PosixThread::waitUntilStarted()
{
    std::unique_lock<std::mutex> lock(startMutex);
    startCV.wait(lock, [this]()
                 { return hasStarted.load(); });
}

void PosixThread::run()
{
    {
        std::lock_guard<std::mutex> lock(startMutex);
        hasStarted.store(true);
    }
    startCV.notify_one();

    while (true)
    {
        // Wait if shouldSleep is true
        std::unique_lock<std::mutex> lock(sleepMutex);
        sleepCV.wait(lock, [this]()
                     { return !shouldSleep.load(); });

        // Run user code
        userFunc();
        break; // remove this if you want looping behavior
    }
}

void PosixThread::setPriority(ThreadPriority priority)
{
    platformSetPriority(priority);
}

void PosixThread::sleep()
{
    shouldSleep.store(true);
}

void PosixThread::wakeup()
{
    shouldSleep.store(false);
    sleepCV.notify_one();
}

void PosixThread::join()
{
    if (thread.joinable())
        thread.join();
}

// Platform-Specific Priority Setting

void PosixThread::platformSetPriority(ThreadPriority priority)
{
#ifdef _WIN32
    int winPriority = THREAD_PRIORITY_NORMAL;
    switch (priority)
    {
    case ThreadPriority::Low:
        winPriority = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case ThreadPriority::High:
        winPriority = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case ThreadPriority::RealTime:
        winPriority = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    default:
        break;
    }
    SetThreadPriority((HANDLE)thread.native_handle(), winPriority);

#elif defined(__linux__) || defined(__APPLE__)
    int policy;
    sched_param sch_params;

    pthread_t handle = pthread_self(); // for Linux/macOS

    switch (priority)
    {
    case ThreadPriority::Low:
        policy = SCHED_OTHER;
        sch_params.sched_priority = 0;
        break;
    case ThreadPriority::Normal:
        policy = SCHED_OTHER;
        sch_params.sched_priority = 0;
        break;
    case ThreadPriority::High:
        policy = SCHED_RR;
        sch_params.sched_priority = 50;
        break;
    case ThreadPriority::RealTime:
        policy = SCHED_FIFO;
        sch_params.sched_priority = 80;
        break;
    }

    pthread_setschedparam(handle, policy, &sch_params);
#endif
}

