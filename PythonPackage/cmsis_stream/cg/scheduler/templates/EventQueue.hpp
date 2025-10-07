/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        EventQueue.hpp
 * Description:  Interface for the event queue handling events in the CMSIS Stream Library
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

/***************
 *
 * Event system
 *
 **************/

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "StreamNode.hpp"

namespace arm_cmsis_stream
{

/* Same computer */
struct LocalDestination
{
    /* Destination node */
    StreamNode *dst;
    /* Destination port */
    int dstPort;
};

/* To network if supported by the application callback */
struct DistantDestination
{
    int32_t src_node_id;
};

struct Message
{
    std::variant<LocalDestination, DistantDestination> destination;
    /* Event */
    Event event;
};

// In case of a threaded implementation
// an implementation of this API should be thread safe
class EventQueue
{
  public:
    using AppHandler = bool (*)(int src_node_id, void *data, Event &&evt);

    /* There is a global event queue for all the graphs that may be
       contained in the application
    */
    inline static EventQueue *cg_eventQueue = nullptr;

    EventQueue() {};
    virtual ~EventQueue() {};

    // In case of a threaded implementation
    // this should wakeup the thread
    // Return false in case of queue overflow
    virtual bool push(Message &&message) = 0;
    virtual bool isEmpty() = 0;
    virtual void clear() = 0;

    // In case of a threaded implementation
    // this should sleep when no more any event are available
    // and wakeup when notified from push or end
    virtual void execute() = 0;

    bool mustEnd() const noexcept
    {
        return (mustEnd_.load());
    };

    // In case of multi-threaded implementation
    // this should wakeup the thread
    virtual void end() noexcept
    {
        mustEnd_.store(true);
    };

    // Set an application handler.
    // Events sent to a null node are sent to the application handler.
    // data is any additional data needed by the handler.
    bool callHandler(int node_id, arm_cmsis_stream::Event &&evt)
    {
        if (mustEnd_)
        {
            return false; // Do not call the handler if we are ending
        }
        if (handlerReady_)
        {
            if (this->handler)
            {
                return (this->handler(node_id, this->handlerData, std::move(evt)));
            }
            return false;
        }
        return false;
    };

    void setHandler(void *data, AppHandler handler)
    {
        handlerData = data;
        this->handler = handler;
        handlerReady_ = true;
    };

  protected:
    void *handlerData = nullptr;
    AppHandler handler = nullptr;
    std::atomic<bool> mustEnd_ = false;
    std::atomic<bool> handlerReady_ = false;
};

class EventOutput
{
  protected:
    enum EventMode
    {
        kSync,
        kAsync
    };

    bool sendEventToAllNodes(Event &&evt,
                             EventMode mode = kSync)
    {
        if ((mode == kAsync) && ((EventQueue::cg_eventQueue == nullptr) || EventQueue::cg_eventQueue->mustEnd()))
        {
            return false;
        }

        if (evt.event_id == kNoEvent)
        {
            return true; // No event to send
        }

        if (mNodes.size() == 1)
        {
            LocalDestination destination = mNodes[0];
            if (mode == kAsync)
            {
                // If async, we just push the event to the queue
                Message msg = {destination, std::move(evt)};
                if (!EventQueue::cg_eventQueue->push(std::move(msg)))
                {
                    // If the queue is full, we return false
                    return false;
                }
            }
            else
            {
                // If not async, we call the processEvent directly
                destination.dst->processEvent(destination.dstPort, std::move(evt));
            }
        }
        else
        {

            for (LocalDestination destination : mNodes)
            {
                if (mode == kAsync)
                {
                    // If async, we just push the event to the queue
                    Message msg = {destination, evt.clone()};
                    if (!EventQueue::cg_eventQueue->push(std::move(msg)))
                    {
                        // If the queue is full, we return false
                        return false;
                    }
                }
                else
                {
                    // If not async, we call the processEvent directly
                    destination.dst->processEvent(destination.dstPort, std::move(evt.clone()));
                }
            }
        }
        return true; // Event sent successfully
    };

  public:
    void subscribe(StreamNode &node, int dstPort = 0)
    {
        mNodes.push_back(LocalDestination{&node, dstPort});
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

    template <typename... Args>
    bool sendAsync(Event &&evt)
    {
        return sendEventToAllNodes(std::move(evt), kAsync);
    }

    template <typename... Args>
    static void sendSyncToApp(int node_id,
                              enum cg_event_priority priority,
                              uint32_t selector,
                              Args &&...args)
    {
        sendToApp(node_id, priority, kSync, selector, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static bool sendAsyncToApp(int node_id,
                               enum cg_event_priority priority,
                               uint32_t selector,
                               Args &&...args)
    {
        return sendToApp(node_id, priority, kAsync, selector, std::forward<Args>(args)...);
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

    template <typename... Args>
    static bool sendToApp(int node_id,
                          enum cg_event_priority priority,
                          EventMode mode,
                          uint32_t selector,
                          Args &&...args)
    {

        if ((mode == kAsync) && ((EventQueue::cg_eventQueue == nullptr) || EventQueue::cg_eventQueue->mustEnd()))
        {
            return false;
        }

        Event evt(selector, priority, std::forward<Args>(args)...);
        if (mode == kAsync)
        {
            Message msg = {DistantDestination{node_id}, std::move(evt)};
            if (!EventQueue::cg_eventQueue->push(std::move(msg)))
            {
                // If the queue is full, we return false
                return false;
            }
            return true;
        }
        else
        {

            return EventQueue::cg_eventQueue->callHandler(node_id, std::move(evt));
        }
    };

    std::vector<LocalDestination> mNodes;
};

} // namespace arm_cmsis_stream
