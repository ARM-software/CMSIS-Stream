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
// Configuration of the event system
// (enabled, multi-threaded, etc.)
#include "config_events.h"

#if defined(CG_EVENTS_MULTI_THREAD)
#define CG_MUTEX std::shared_mutex
#define CG_INIT_MUTEX(MUTEX)
#define CG_MUTEX_DELETE(MUTEX, ERROR)
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

// When no multithreading, event queue is processed after each node execution
// Otherwise, it is processed in a separate thread as defined in your main application.
#if !defined(CG_EVENTS_MULTI_THREAD)
#define CG_BEFORE_NODE_EXECUTION(id) \
  arm_cmsis_stream::EventQueue::cg_eventQueue->execute();
#endif

// At the end of the schedule, clear the event queue
// We should not process events after the schedule has ended since the nodes may been
// destroyed. (If node initializations is done in the schedule function).
#define CG_AFTER_SCHEDULE                                 \
  if (arm_cmsis_stream::EventQueue::cg_eventQueue)        \
  {                                                       \
    arm_cmsis_stream::EventQueue::cg_eventQueue->clear(); \
  }
#endif

#include <mutex>
#include <shared_mutex>

#include <memory_resource>

extern std::pmr::synchronized_pool_resource pool;

#define CG_MK_LIST_EVENT_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))

#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))

#define CG_MK_PROTECTED_MUTEX_ALLOCATOR(T) (std::pmr::polymorphic_allocator<T>(&pool))
