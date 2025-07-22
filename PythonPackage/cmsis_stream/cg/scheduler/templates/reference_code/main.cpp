/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        main.cpp
 * Description:  Example implementation to demontrate the CMSIS Stream Library.
 *               Data flow and events are handled.
 *               Can be configured for bare metal or multi-threaded environments.
 *               The multi-threaded environment is implemented using C++ mutex and posix threads.
 *               And RTOS can provide a totally different implementation.
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
#include <cstdio>
#include <cstdint>
#include "cstream_node.h"
#include "scheduler.h"
#include "cg_enums.h"

#include "config_events.h"
#include "custom.hpp"
#include "posix_thread.hpp"
#include "cg_queue.hpp"

#include <iostream>

// When event system is enabled, we provide a global event queue
#if defined(CG_EVENTS)

// When event multi-threading is enabled, we define a thread to process the event queue
#if defined(CG_EVENTS_MULTI_THREAD)
PlatformThread *cg_eventThread = nullptr;
bool cg_eventThreadDone = false;


#endif // CG_MULTI_THREAD


#endif // CG_EVENTS

using namespace arm_cmsis_stream;


int main(int argc, char const *argv[])
{
    int err = init_scheduler();
    if (err == CG_MEMORY_ALLOCATION_FAILURE)
    {
        std::cerr << "Error: Memory allocation failure during scheduler initialization." << std::endl;
        return -1;
    }
#if defined(CG_EVENTS)

    arm_cmsis_stream::EventQueue::cg_eventQueue = new MyQueue();

#if defined(CG_EVENTS_MULTI_THREAD)

   
    // Thread is processing the event queue an sleeping when no events are available
    PosixThread t([]
                  {
        std::cout << "Thread started!" << std::endl;
        // If we are done with the scheduling, we exit the thread
        while(!cg_eventThreadDone)
        {
            arm_cmsis_stream::EventQueue::cg_eventQueue->execute();
            if (cg_eventThread)
               cg_eventThread->sleep();
        } });

    // Thread is started and we wait until the thread is ready
    // before we start the scheduler.
    cg_eventThread = &t;
    t.start();
    t.setPriority(ThreadPriority::Low);
    t.waitUntilStarted(); // Wait until the thread is ready to process events
#endif

#endif

    // Initialize and start the scheduler
    int error;
    printf("Start\n");
    uint32_t nbSched = scheduler(&error);
    printf("Nb sched = %d\n", nbSched);
    if (error != CG_SUCCESS)
    {
        std::cerr << "Error during scheduling: " << error << std::endl;
        return -1;
    }
#if defined(CG_EVENTS)
#if defined(CG_EVENTS_MULTI_THREAD)

    // Notify the thread we are done and wait for the end
    // of the thread.
    cg_eventThreadDone = true;
    t.wakeup(); // Wake up the thread to finish processing
    t.join();


#endif


    // Delete the event queue
    delete arm_cmsis_stream::EventQueue::cg_eventQueue;
    arm_cmsis_stream::EventQueue::cg_eventQueue = nullptr;
#endif
    free_scheduler();
    return 0;
}