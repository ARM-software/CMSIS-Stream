/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cg_queue.cpp
 * Description:  Example implementation of a queue for handling events using C++ mutexes
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
#include "config_events.h"
#include "custom.hpp"
#include "posix_thread.hpp"
#include "cg_queue.hpp"

bool MyQueue::push(arm_cmsis_stream::Message &&event)
{
    bool res = true;
    CG_MUTEX_ERROR_TYPE err;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, err);
    if (nb_elems < MY_QUEUE_MAX_ELEMS)
    {
        nb_elems++;
        queue.push(std::move(event));
    }
    else
    {
        res = false;
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, err);
    if (res)
    {
#if defined(CG_EVENTS) && defined(CG_EVENTS_MULTI_THREAD)
        if (cg_eventThread)
            cg_eventThread->wakeup(); // Notify the thread to process the queue
#endif
    }
    return res;
    // std::cout << "Event pushed to queue: " << event.event.event_id << std::endl;
}

bool MyQueue::isEmpty()
{
    bool r;
    CG_MUTEX_ERROR_TYPE err;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, err);
    r = queue.empty();
    CG_EXIT_CRITICAL_SECTION(queue_mutex, err);
    return r;
}

void MyQueue::clear()
{
    CG_MUTEX_ERROR_TYPE err;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, err);
    while (!queue.empty())
    {
        queue.pop();
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, err);
}


void MyQueue::execute()
{
    bool empty = false;
    CG_MUTEX_ERROR_TYPE err;
    while (!empty)
    {

        arm_cmsis_stream::Message msg;
        bool messageWasRead = false;

        // processEvent may push in async way some new events.
        // So the lock must eb released before
        // process event is called.
        CG_ENTER_CRITICAL_SECTION(queue_mutex, err);
        empty = queue.empty();
        if (!empty)
        {
            msg = std::move(queue.front());
            queue.pop();
            nb_elems--;
            messageWasRead = true;
        }
        CG_EXIT_CRITICAL_SECTION(queue_mutex, err);

        if (messageWasRead)
        {
            if (msg.dst)
            {
                msg.dst->processEvent(msg.dstPort, msg.event);
            }
            else
            {
                // Application handler
                this->callHandler(msg.event);
            }
        }
    }
}

