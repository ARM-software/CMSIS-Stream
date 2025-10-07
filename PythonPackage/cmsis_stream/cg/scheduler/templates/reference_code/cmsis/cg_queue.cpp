/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cg_queue.cpp
 * Description:  Example implementation of a queue for handling events using C++ mutexes
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2021-2025 ARM Limited or its affiliates. All rights reserved.
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

#include "cg_queue.hpp"
#include "config.h"
#include "custom.hpp"
#include <cstdint>
#include <variant>

extern osThreadId_t cg_eventThread;

using namespace arm_cmsis_stream;

MyQueue::MyQueue(osPriority_t low, osPriority_t normal, osPriority_t high)
    : arm_cmsis_stream::EventQueue()
{
    priorities[0] = low;
    priorities[1] = normal;
    priorities[2] = high;
    for (uint32_t p = 0; p < nb_priorities; p++)
    {
        queue[p] = new (std::nothrow) Message[MY_QUEUE_MAX_ELEMS];
        read[p] = 0;
        write[p] = 0;
        nb_elems[p] = 0;
    }
}

MyQueue::~MyQueue()
{
    for (uint32_t p = 0; p < nb_priorities; p++)
    {
        delete[] queue[p];
    }
}

bool MyQueue::push(arm_cmsis_stream::Message &&event)
{
    bool ok = false;
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    if (!CG_MUTEX_HAS_ERROR(error))
    {
        uint32_t p = event.event.priority;
        if (p >= nb_priorities)
        {
            p = nb_priorities - 1; // Highest priority
        }
        if (nb_elems[p] < MY_QUEUE_MAX_ELEMS)
        {
            //DEBUG_PRINT("Push event %d\n", event.event.event_id);
            queue[p][write[p]++] = std::move(event);
            if (write[p] == MY_QUEUE_MAX_ELEMS)
            {
                write[p] = 0; // Wrap around
            }

            nb_elems[p]++;
            ok = true;
        }
        else {
            ERROR_PRINT("Event queue overflow for priority %d\n", p);
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
    if (cg_eventThread != nullptr)
    {
        osThreadFlagsSet(cg_eventThread, MY_QUEUE_NEW_EVENT_FLAG);
    }

    return ok;
}

bool MyQueue::isEmpty()
{
    bool r = true;
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    for (uint32_t p = 0; p < nb_priorities; p++)
    {
        if (nb_elems[p] != 0)
        {
            r = false;
            break;
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
    return r;
}

void MyQueue::clear()
{
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    if (!CG_MUTEX_HAS_ERROR(error))
    {
        for (uint32_t p = 0; p < nb_priorities; p++)
        {

            while (nb_elems[p] != 0)
            {
                Message msg = std::move(queue[p][read[p]++]);
                if (read[p] == MY_QUEUE_MAX_ELEMS)
                {
                    read[p] = 0; // Wrap around
                }
                nb_elems[p]--;
                msg = Message(); // Reset the message
            }
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
}

void MyQueue::end() noexcept
{
    mustEnd_.store(true);
    if (cg_eventThread != nullptr)
    {
        osThreadFlagsSet(cg_eventThread, MY_QUEUE_NEW_EVENT_FLAG);
    }
};

// The thread priority will be changed according to the event priority
// The thread priority is always the highest to extract events from the queue
// and then change to the event priority to process the event
void MyQueue::execute()
{
    CG_MUTEX_ERROR_TYPE error;
    while (!this->mustEnd())
    {
        while ((!this->mustEnd()) && (!isEmpty()))
        {
            Message msg;
            bool messageWasReceived = false;
            CG_ENTER_CRITICAL_SECTION(queue_mutex, error);

            if (!CG_MUTEX_HAS_ERROR(error))
            {
                for (int32_t p = nb_priorities - 1; p >= 0; p--)
                {
                    if (nb_elems[p] != 0)
                    {
                        msg = std::move(queue[p][read[p]++]);
                        if (read[p] == MY_QUEUE_MAX_ELEMS)
                        {
                            read[p] = 0; // Wrap around
                        }

                        nb_elems[p]--;
                        messageWasReceived = true;
                        break;
                    }
                }
            }
            CG_EXIT_CRITICAL_SECTION(queue_mutex, error);

            // Process event with no lock held
            if (messageWasReceived)
            {
                osThreadId_t tid = osThreadGetId();
                int p = msg.event.priority;
                if (p >= nb_priorities)
                {
                    p = nb_priorities - 1; // Highest priority
                }
                osThreadSetPriority(tid, priorities[p]);
                if (std::holds_alternative<LocalDestination>(msg.destination))
                {
                    LocalDestination &local = std::get<LocalDestination>(msg.destination);
                    local.dst->processEvent(local.dstPort, std::move(msg.event));
                }
                else if (std::holds_alternative<DistantDestination>(msg.destination))
                {
                    DistantDestination &dist = std::get<DistantDestination>(msg.destination);
                    this->callHandler(dist.src_node_id, std::move(msg.event));
                }
                osThreadSetPriority(tid, priorities[nb_priorities - 1]); // Back to highest priority
            }
        }
        if (this->mustEnd())
        {
            return;
        }
        // If new event was pushed and missed with the
        // empty test
        osThreadFlagsWait(MY_QUEUE_NEW_EVENT_FLAG, osFlagsWaitAny, osWaitForever);
    }
}
