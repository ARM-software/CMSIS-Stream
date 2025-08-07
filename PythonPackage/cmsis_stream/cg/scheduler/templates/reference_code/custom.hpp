/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        custom.hpp
 * Description:  Example configuration for CMSIS-Stream with event handling in bare metal
 *               and multi-threaded environments.
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
// A file for configuration of the event system must be created
// (enabled, multi-threaded, etc.)
//#include "config_events.h"


#if defined(CG_EVENTS_MULTI_THREAD)
// Define macros for the mutex handling here
#endif

// When no multithreading, event queue is processed after each node execution
// Otherwise, it is processed in a separate thread as defined in your main application.
#if !defined(CG_EVENTS_MULTI_THREAD)
#define CG_BEFORE_NODE_EXECUTION(id)                        \
  if (arm_cmsis_stream::EventQueue::cg_eventQueue)          \
  {                                                         \
    arm_cmsis_stream::EventQueue::cg_eventQueue->execute(); \
  }
#endif

// At the end of the schedule, clear the event queue
// We should not process events after the schedule has ended since the nodes may been
// destroyed. (If node initializations is done in the schedule function).
#define CG_AFTER_SCHEDULE                                 \
  if (arm_cmsis_stream::EventQueue::cg_eventQueue)        \
  {                                                       \
    arm_cmsis_stream::EventQueue::cg_eventQueue->clear(); \
  }

// You may define macros to customize the memory allocators here
