/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        EventQueue.h
 * Description:  Interface for the event queue handling events in the CMSIS Stream Library
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
#pragma once

/***************
 *
 * Event system
 *
 **************/

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include "StreamNode.hpp"

namespace arm_cmsis_stream
{

    struct Message
    {
        /* Destination node */
        StreamNode *dst;
        /* Destination port */
        int dstPort;
        /* Event */
        Event event;
    };

    // In case of a threaded implementation
    // an implementation of this API should be thread safe
    class EventQueue
    {
    public:
#if defined(CG_EVENTS)
    /* There is a global event queue for all the graphs that may be
       contained in the application
    */
    inline static EventQueue *cg_eventQueue=nullptr;
#endif
        EventQueue() {};
        virtual ~EventQueue() {};

        // In case of a threaded implementation
        // this should wakeup the thread
        // Return false in case of queue overflow
        virtual bool push(Message &&event) = 0;
        virtual bool isEmpty() = 0;
        virtual void clear() = 0;

        // In case of a threaded implementation
        // this should sleep when no more any event are available
        virtual void execute() = 0;
    };



    class EventOutput
    {
    protected:
        enum EventMode {
           kSync,
           kAsync
        };

        bool sendEventToAllNodes(Event &&evt,
                                 EventMode mode = kSync)
        {
#if defined(CG_EVENTS)
            if ((evt.event_id == kNoEvent) || (EventQueue::cg_eventQueue == nullptr))
            {
                return true; // No event to send
            }

            for (std::pair<StreamNode *, int> pair : mNodes)
            {
                if (mode == kAsync)
                {
                    // If async, we just push the event to the queue
                    Message msg = {std::get<0>(pair), std::get<1>(pair), evt};
                    if (!EventQueue::cg_eventQueue->push(std::move(msg)))
                    {
                        // If the queue is full, we return false
                        return false;
                    }
                }
                else
                {
                    // If not async, we call the processEvent directly
                    std::get<0>(pair)->processEvent(std::get<1>(pair), evt);
                }
            }

            return true; // Event sent successfully
#endif
        };

    public:
        void subscribe(StreamNode &node, int dstPort = 0)
        {
            mNodes.push_back(std::make_pair(&node, dstPort));
        };

        template <typename... Args>
        void sendSync(enum cg_event_priority priority,
                      uint32_t selector,
                      Args &&...args)
        {
            sendCombinedValue(priority, kSync, selector, std::forward<Args>(args)...);
        }

        template <typename... Args>
        bool sendAsync(enum cg_event_priority priority,
                       uint32_t selector,
                       Args &&...args)
        {
            return sendCombinedValue(priority, kAsync, selector, std::forward<Args>(args)...);
        }

    protected:
        template <typename... Args>
        bool sendCombinedValue(enum cg_event_priority priority,
                               EventMode mode,
                               uint32_t selector,
                               Args &&...args)
        {
            // When more than one value
            // we create a combined event
            Event evt(selector, priority, std::forward<Args>(args)...);
            return sendEventToAllNodes(std::move(evt), mode);
        };

        std::vector<std::pair<StreamNode *, int>> mNodes;
    };

} // end cmsis stream namespace

