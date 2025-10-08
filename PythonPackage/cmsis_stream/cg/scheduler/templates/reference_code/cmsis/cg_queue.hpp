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
#ifndef CG_QUEUE_H
#define CG_QUEUE_H

extern "C"
{
#include "RTE_Components.h"
#include CMSIS_device_header

#include "cmsis_os2.h" /* CMSIS-RTOS2 API */
}

#include "cg_enums.h"
#include "custom.hpp"
#include "StreamNode.hpp"
#include "EventQueue.hpp"
#include <utility>

#ifndef MY_QUEUE_MAX_ELEMS
#define MY_QUEUE_MAX_ELEMS 20
#endif

#define MY_QUEUE_NEW_EVENT_FLAG 0x1 

class MyQueue:public arm_cmsis_stream::EventQueue
{
public:
    MyQueue(osPriority_t low, osPriority_t normal, osPriority_t high);
    ~MyQueue();

    // This should wakeup the threads processing this queue
    bool push(arm_cmsis_stream::Message &&event)  final;
    bool isEmpty()  final;
    void clear()  final;
    // This should sleep when there is no more any event to process
    void execute()  final;
    void end() noexcept final;


private:
    CG_MUTEX queue_mutex;
    
protected:
    constexpr static uint32_t nb_priorities = 3;
    // Several queues for the three priorities supported
    // by the event system.
    arm_cmsis_stream::Message *queue[nb_priorities];
    int read[nb_priorities],write[nb_priorities];

    uint32_t nb_elems[nb_priorities];

    osPriority_t priorities[nb_priorities];
};


#endif
