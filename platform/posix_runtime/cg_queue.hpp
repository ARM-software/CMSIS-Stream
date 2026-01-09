/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cg_queue.h
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
#pragma once


#include "cg_enums.h"
#include "StreamNode.hpp"
#include "EventQueue.hpp"
#include <utility>
#include "posix_thread.hpp"

#ifndef MY_QUEUE_MAX_ELEMS
#define MY_QUEUE_MAX_ELEMS 20
#endif



class MyQueue:public arm_cmsis_stream::EventQueue
{
public:
    MyQueue();
    ~MyQueue();

    // This should wakeup the threads processing this queue
    bool push(arm_cmsis_stream::Message &&event)  final;
    bool isEmpty()  final;
    void clear()  final;
    // This should sleep when there is no more any event to process
    void execute()  final;
    void end() noexcept final;

    void setThread(PosixThread *t)
    {
        cg_eventThread_ = t;
    };


private:
    CG_MUTEX queue_mutex;
    std::condition_variable cv;
    bool event=false;
    std::mutex cv_m;

    

    void waitEvent()
    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [this]{return event;});
        event = false;
    }

    void notifyQueue()
    {
        {
           std::lock_guard<std::mutex> lk(cv_m);
           event = true;
        }
        cv.notify_one();
    }
    
protected:
    constexpr static uint32_t nb_priorities = 3;
    // Several queues for the three priorities supported
    // by the event system.
    arm_cmsis_stream::Message *queue[nb_priorities];
    int read[nb_priorities],write[nb_priorities];

    uint32_t nb_elems[nb_priorities];
    PosixThread *cg_eventThread_ = nullptr;

};



