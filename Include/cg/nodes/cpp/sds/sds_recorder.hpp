/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        sds_recorder.hpp
 * Description:  Node for SDS Recorder
 *
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
#pragma once

#include "cmsis_os2.h"
#include "sds.h"
#include "sds_rec.h"
#include "cg_status.h"

/*

SDS Recorder node to work with the
https://github.com/ARM-software/SDS-Framework

Synchronous Data Stream (SDS) Framework allowing 
to record real-world data off a device and playing 
it back on Arm Virtual Hardware.

*/

template<typename IN,int inputSize>
class SDSRecorder: GenericSink<IN,inputSize>
{
public:
    SDSRecorder(FIFOBase<IN> &src,
                sds_recorder_cg_connection_t *sdsConnection):
    GenericSink<IN,inputSize>(src)
    {
        if (sdsConnection)
        {
            mRecId = sdsRecOpen(sdsConnection->sensorName,
                                sdsConnection->recorderBuffer,
                                sdsConnection->recorderBufferSize,
                                sdsConnection->recorderThreshold);
        }
        else
        {
            mRecId=NULL;
        }
    };

    ~SDSRecorder()
    {
        if (mRecId != NULL)
        {
           sdsRecClose(mRecId); 
        }
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        IN *b=this->getReadBuffer();
        if (mRecId == NULL)
        {
            return(CG_INIT_FAILURE);
        }

        uint32_t timestamp;
        timestamp = osKernelGetTickCount();


        /*

        Try to write all the input buffer to the SDS recorder
        stream.

        The timestamp used corresponds to when this SDS recorder node
        is executed. This time may be different from the time when
        the samples have been received by the CMSIS-Stream graph.

        SDS framework is recovering timestamps using the known
        sampling frequency of the sensors so in general it is
        not a problem if this timestamp used here is not accurate.

        */
        uint32_t num = sdsRecWrite(mRecId, timestamp, b, inputSize*sizeof(IN));
        
        /*

        Check for SDS Recorder overflow

        */
        if (num != inputSize*sizeof(IN)) {
            return(CG_BUFFER_OVERFLOW);
        }
        return(CG_SUCCESS);
    };

protected:
    sdsRecId_t mRecId;
};

