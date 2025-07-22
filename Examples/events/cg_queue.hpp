/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cg_queue.hpp
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
#pragma once

#include <queue>
#include <mutex>

#include "cg_enums.h"
#include "config_events.h"
#include "custom.hpp"
#include "StreamNode.hpp"
#include "EventQueue.hpp"

#ifndef MY_QUEUE_MAX_ELEMS
#define MY_QUEUE_MAX_ELEMS 1000
#endif

class MyQueue:public arm_cmsis_stream::EventQueue
{
public:
    MyQueue() = default;
    ~MyQueue() = default;

    // This should wakeup the threads processing this queue
    bool push(arm_cmsis_stream::Message &&message)  final;
    bool isEmpty()  final;
    void clear()  final;
    // This should sleep when there is no more any event to process
    void execute()  final;
    

private:
#if defined(CG_EVENTS) && defined(CG_EVENTS_MULTI_THREAD)
    CG_MUTEX queue_mutex;
#endif
    std::queue<arm_cmsis_stream::Message> queue;
    uint32_t nb_elems=0;
    
};

