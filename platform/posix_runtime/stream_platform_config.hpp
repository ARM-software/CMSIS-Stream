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
#pragma once


// Needed for pure event graphs to avoid having
// an infinite loop generated to process dataflow when
// there is no dataflow nodes.
#define CG_EVENTS_MULTI_THREAD

#define CG_MUTEX std::shared_mutex
#define CG_MUTEX_ERROR_TYPE int
#define CG_MUTEX_HAS_ERROR(ERROR) (ERROR != 0)

#define CG_ENTER_CRITICAL_SECTION(MUTEX, ERROR) \
  {                                             \
    std::unique_lock lock((MUTEX));             \
    ERROR = 0;

#define CG_EXIT_CRITICAL_SECTION(MUTEX, ERROR) \
  ERROR = 0;                                   \
  }

#define CG_ENTER_READ_CRITICAL_SECTION(MUTEX, ERROR) \
  {                                                  \
    std::shared_lock lock((MUTEX));                  \
    ERROR = 0;

#define CG_EXIT_READ_CRITICAL_SECTION(MUTEX, ERROR) \
  ERROR = 0;                                        \
  }


// Because memory optimization is enabled
#define CG_BEFORE_BUFFER alignas(16)

// Can be used to detect overflow / underflow error in audio driver for instance
// and stop the dataflow execution
// Or can be used for debugging the dataflow and print the node ID before execution
//#define CG_BEFORE_NODE_EXECUTION(ID) 

#include <mutex>
#include <shared_mutex>

#include <memory_resource>

extern std::pmr::synchronized_pool_resource pool;

#define CG_MK_LIST_EVENT_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))

#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))

#define CG_MK_PROTECTED_MUTEX_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))

#include <chrono>
#include <cstdint>

static inline uint32_t monotonic_ms_u32(void)
{
    using namespace std::chrono;

    auto now = steady_clock::now().time_since_epoch();
    auto ms  = duration_cast<milliseconds>(now).count();

    return static_cast<uint32_t>(ms); // wraps naturally
}

#define CG_TIME_STAMP_TYPE uint32_t

#define CG_GET_TIME_STAMP()  monotonic_ms_u32()  

#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__);
#define LOG_DBG(...) fprintf(stderr, __VA_ARGS__);

#define CG_TENSOR_NB_DIMS 2

#define CG_MAX_VALUES 4