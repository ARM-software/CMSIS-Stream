/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        sds_cg_types.hpp
 * Description:  Node for interacting between CMSIS-Stream SDS nodes
 *               and application
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
 *
 * Copyright (C) 2021-2023 ARM Limited or its affiliates. All rights reserved.
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
#ifndef _SDS_CG_TYPES_H_
#define _SDS_CG_TYPES_H_

#include "cmsis_os2.h"
#include "sds.h"

#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct {
    uint32_t event; /* CMSIS RTOS event ID for this sensor */
    uint32_t cancel_event; /* CMSIS RTOS event ID for all sensors */
    uint32_t timeout; /* Timeout to wait for an event */
    sdsId_t sdsId; /* SDS stream ID */
} sds_sensor_cg_connection_t;


typedef struct {
    const char *sensorName; /* Name of sensor */
    uint8_t *recorderBuffer; /* Recorder buffer descriptions */
    uint32_t recorderBufferSize;
    uint32_t recorderThreshold;
} sds_recorder_cg_connection_t;

/* Drift delegate API
   Very similar to the sdsRead API.

   Must generate at most buf_size data when called.
   If not enough data is generated, the function will 
   be called again.

   CMSIS RTOS events are managed by the SDS Sensor node.
   This function only need to focus on the data processing
*/
typedef uint32_t (*driftDelegate_t) (sdsId_t id, 
                                     void *buf, 
                                     uint32_t buf_size,
                                     void *delegate_data);



#ifdef   __cplusplus
}
#endif

#endif