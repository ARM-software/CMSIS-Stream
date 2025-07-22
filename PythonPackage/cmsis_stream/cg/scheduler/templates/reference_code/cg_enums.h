/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cg_enums.h
 * Description:  Enums for CMSIS Stream
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

#ifndef CG_ENUM_H_
#define CG_ENUM_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /* The values CG_SUCCESS, CG_SKIP_EXECUTION,CG_BUFFER_ERROR
    must be preserved.

    Other error codes can be redefined by the user since they
    are not used by the core nodes.

    */

    typedef enum
    {
        CG_SUCCESS = 0,                    /**< No error */
        CG_BUFFER_UNDERFLOW = -1,          /**< FIFO underflow */
        CG_BUFFER_OVERFLOW = -2,           /**< FIFO overflow */
        CG_MEMORY_ALLOCATION_FAILURE = -3, /**< Memory allocation failure */
        CG_INIT_FAILURE = -4,              /**< Node initialization failure */
        CG_SKIP_EXECUTION = -5,            /**< Skip node execution (asynchronous mode) */
        CG_BUFFER_ERROR = -6,              /**< Stop execution due to FIFO overflow or underflow (asynchronous mode for pure function) */
        CG_OS_ERROR = -7,                  /**< RTOS API error */
        CG_STOP_SCHEDULER = -8,            /**< Stop scheduling */
        CG_PAUSED_SCHEDULER = -9           /**< Pause scheduler in callback mode */
    } cg_status;

    enum cg_standard_event
    {
        kNoEvent = 0,
        kDo = 1,
        kPause = 2,
        kResume = 3,
        kGetParam = 5,
        kValue = 6,
        kStopGraph = 7,
        kDebug = 8,
        kStartNodeInitialization = 9,     
        kNodeWasInitialized = 10,    
        kNodeWillBeDestroyed = 11,
        kNodeReadyToBeDestroyed = 12,      
        // Node selectors are starting at 100
        kSelector = 100
    };

    enum cg_event_priority
    {
        kLowPriority = 0,
        kNormalPriority = 1,
        kHighPriority = 2
    };

#ifdef __cplusplus
}
#endif

#endif /* CG_ENUM_H_ */