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
#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

/***************
 *
 * Event system
 *
 **************/

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace arm_cmsis_stream
{

    struct Message
    {
        StreamNode &dst;
        int dstPort;
        Event event;
    };

    class EventQueue
    {
    public:
        EventQueue() {};

        virtual void push(Message &&event) = 0;
        virtual bool isEmpty() = 0;
        virtual void clear() = 0;
        virtual void execute() = 0;
    };

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

    extern EventQueue *cg_eventQueue;
#if defined(CG_EVENTS_MULTI_THREAD)
    extern PlatformThread *cg_eventThread;
#endif

    class EventOutput
    {
    protected:
        void sendEventToAllNodes(Event &&evt, 
                                 bool async = false)
        {
            if ((evt.event_id == kNoEvent) || (cg_eventQueue == nullptr))
            {
                return; // No event to send
            }
            if (mNodes.size() == 1)
            {
                auto pair = mNodes[0];
                // If the event is sent to a single node, we can move it 
                if (async)
                {
                    // If async, we just push the event to the queue
                    Message msg = {*std::get<0>(pair), std::get<1>(pair), std::move(evt)};
                    cg_eventQueue->push(std::move(msg));
                }
                else
                {
                    // If not async, we call the processEvent directly
                    std::get<0>(pair)->processEvent(std::get<1>(pair), std::move(evt));
                }
            }
            else 
            {
                // We have to copy the event if it is sent to multiple nodes
                for (std::pair<StreamNode *, int> pair : mNodes)
                {
                    if (async)
                    {
                        // If async, we just push the event to the queue
                        Message msg = {*std::get<0>(pair), std::get<1>(pair), evt};
                        cg_eventQueue->push(std::move(msg));
                    }
                    else
                    {
                        // If not async, we call the processEvent directly
                        std::get<0>(pair)->processEvent(std::get<1>(pair), evt);
                    }
                }
          }
        };

    public:
        void subscribe(StreamNode &node, int dstPort = 0)
        {
            mNodes.push_back(std::make_pair(&node, dstPort));
        };

        void sendDo(enum cg_event_priority priority = kNormalPriority,
                    bool async = false)
        {
            Event evt(kDo, cg_value(),priority);
            sendEventToAllNodes(std::move(evt), async);
        };

        void sendPause(enum cg_event_priority priority = kNormalPriority,
                       bool async = false)
        {
            Event evt(kPause, cg_value(),priority);
            sendEventToAllNodes(std::move(evt), async);
        };

        void sendResume(enum cg_event_priority priority = kNormalPriority,
                        bool async = false)
        {
            Event evt(kResume, cg_value(),priority);
            sendEventToAllNodes(std::move(evt), async);
        };

        // When this event is received a node will generally generate an event
        // paramValue to return the value
        // So the node must have an event input AND and event output
        void sendGetParam(uint32_t paramID, 
                          enum cg_event_priority priority = kNormalPriority,
                          bool async = false)
        {
            Event evt(kGetParam, cg_value(paramID),priority);
            sendEventToAllNodes(std::move(evt), async);
        };

       

        void sendValue(const cg_value &paramValue, 
                       enum cg_event_priority priority = kNormalPriority,
                       bool async = false)
        {
            Event evt(kValue, paramValue,priority);
            sendEventToAllNodes(std::move(evt), async);
        };


    protected:
        std::vector<std::pair<StreamNode *, int>> mNodes;
    };

    class LockScope
    {
    public:
        virtual ~LockScope() = default;
        virtual void lock() = 0;
        virtual void unlock() = 0;
    };

    class NoMutex : public LockScope
    {
    public:
        void lock() final {}
        void unlock() final {}
    };

    class CPPMutex : public LockScope
    {
    public:
        CPPMutex() = default;
        ~CPPMutex() override = default;

        void lock() override
        {
            m_mutex.lock();
        }

        void unlock() override
        {
            m_mutex.unlock();
        }

    private:
        std::mutex m_mutex;
    };

    class LockGuard
    {
    public:
        explicit LockGuard(LockScope &lockable)
            : m_lockable(lockable)
        {
            m_lockable.lock();
        }

        ~LockGuard()
        {
            m_lockable.unlock();
        }

        // Non-copyable
        LockGuard(const LockGuard &) = delete;
        LockGuard &operator=(const LockGuard &) = delete;

    private:
        LockScope &m_lockable;
    };

} // end cmsis stream namespace

#endif
