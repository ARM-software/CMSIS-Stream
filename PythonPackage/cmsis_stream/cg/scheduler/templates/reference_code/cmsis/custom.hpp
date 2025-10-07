/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        custom.h
 * Description:  Example configuration for CMSIS-Stream with event handling in bare metal
 *               and multi-threaded environments.
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
#ifndef CUSTOM_H_
#define CUSTOM_H_

extern "C"
{
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "config.h"
#include "arm_math_types.h"
}

struct cf32 {
    float real;
    float imag;
};

struct sf32 {
    float left;
    float right;
};

struct cq15 {
    q15_t real;
    q15_t imag;
};

struct sq15 {
    q15_t left;
    q15_t right;
};

class CMSISMutex
{
  public:
    CMSISMutex()
    {
        mutex_id = osMutexNew(NULL);
    }

    ~CMSISMutex()
    {
        if (mutex_id != nullptr)
        {
            // Ensure the mutex is deleted only if it was created successfully
            osMutexDelete(mutex_id);
        }
    }

    osMutexId_t id() const
    {
        return mutex_id;
    }

  protected:
    osMutexId_t mutex_id;
};

class CMSISLock
{
  public:
    CMSISLock(CMSISMutex &mutex)
        : mutex(mutex)
    {
    }

    osStatus_t acquire()
    {
        error = osMutexAcquire(mutex.id(), osWaitForever);
        return error;
    }

    osStatus_t tryAcquire()
    {
        error = osMutexAcquire(mutex.id(), 0);
        return error;
    }

    ~CMSISLock()
    {
        if (error == osOK)
        {
            error = osMutexRelease(mutex.id());
        }
    }

    osStatus_t getError() const
    {
        return error;
    }

  protected:
    CMSISMutex &mutex;
    osStatus_t error;
};

#define CG_MUTEX CMSISMutex
#define CG_MUTEX_ERROR_TYPE osStatus_t

#define CG_MUTEX_HAS_ERROR(ERROR) (ERROR != osOK)

#define CG_ENTER_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                           \
        CMSISLock lock((MUTEX));                \
        ERROR = lock.acquire();

#define CG_EXIT_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_ENTER_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                                \
        CMSISLock lock((MUTEX));                     \
        ERROR = lock.acquire();

#define CG_EXIT_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_MK_LIST_EVENT_ALLOCATOR(T) (CMSISEventPoolAllocator<T>{})
#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) (CMSISBufPoolAllocator<T>{})
#define CG_MK_PROTECTED_MUTEX_ALLOCATOR(T) (CMSISMutexPoolAllocator<T>{})

#include "cmsis_allocator.hpp"

// Queue implementation for events
#include "cg_queue.hpp"

#include "rtos_events.hpp"

// Because memory optimization is enabled
#define CG_BEFORE_BUFFER __ALIGNED(16)

#define CG_BEFORE_SCHEDULE \
  uint32_t errorFlags = 0;

#define CG_BEFORE_NODE_EXECUTION(ID)                                                                             \
{                                                                                                                \
    errorFlags = osThreadFlagsWait(AUDIO_SINK_UNDERFLOW_EVENT | AUDIO_SOURCE_OVERFLOW_EVENT, osFlagsWaitAny, 0); \
    if (errorFlags & AUDIO_SINK_UNDERFLOW_EVENT)                                                                 \
    {                                                                                                            \
        cgStaticError = CG_BUFFER_UNDERFLOW;                                                                     \
        goto errorHandling;                                                                                      \
    }                                                                                                            \
    if (errorFlags & AUDIO_SOURCE_OVERFLOW_EVENT)                                                                \
    {                                                                                                            \
        cgStaticError = CG_BUFFER_OVERFLOW;                                                                      \
        goto errorHandling;                                                                                      \
    }                                                                                                            \
}

#endif