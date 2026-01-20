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



class ZephyrMutex
{
  public:
    ZephyrMutex()
    {
        k_mutex_init(&my_mutex);
    }

    ~ZephyrMutex()
    {
        // No explicit deinitialization needed for k_mutex
    }

    struct k_mutex* id() 
    {
        return &my_mutex;
    }

  protected:
    struct k_mutex my_mutex;
};

class ZephyrLock
{
  public:
    ZephyrLock(ZephyrMutex &mutex)
        : mutex(mutex)
    {
    }

    int acquire()
    {
        error = k_mutex_lock(mutex.id(), K_FOREVER);
        return error;
    }

    int tryAcquire()
    {
        error = k_mutex_lock(mutex.id(), K_NO_WAIT);
        return error;
    }

    ~ZephyrLock()
    {
        if (error == 0)
        {
             k_mutex_unlock(mutex.id());
        }
    }

    int getError() const
    {
        return error;
    }

  protected:
    ZephyrMutex &mutex;
    int error;
};

#define CG_TENSOR_NB_DIMS CONFIG_CMSISSTREAM_TENSOR_MAX_DIMENSIONS
#define CG_MAX_VALUES CONFIG_CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS

// Needed for pure event graphs to avoid having
// the infinite loop generated in single thread mode
// to process events from the stream thread.
#define CG_EVENTS_MULTI_THREAD

#define CG_MUTEX ZephyrMutex
#define CG_MUTEX_ERROR_TYPE int

#define CG_MUTEX_HAS_ERROR(ERROR) (ERROR != 0)

#define CG_ENTER_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                           \
        ZephyrLock lock((MUTEX));                \
        ERROR = lock.acquire();

#define CG_EXIT_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_ENTER_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    {                                                \
        ZephyrLock lock((MUTEX));                     \
        ERROR = lock.acquire();

#define CG_EXIT_READ_CRITICAL_SECTION(MUTEX, ERROR) \
    }

#define CG_MK_LIST_EVENT_ALLOCATOR(T) (ZephyrEventPoolAllocator<T>{})
#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) (ZephyrBufPoolAllocator<T>{})
#define CG_MK_PROTECTED_MUTEX_ALLOCATOR(T) (ZephyrMutexPoolAllocator<T>{})

#include "stream_zephyr_allocator.hpp"

#define CG_TIME_STAMP_TYPE uint32_t

#define CG_GET_TIME_STAMP()  k_cycle_get_32()  

#endif