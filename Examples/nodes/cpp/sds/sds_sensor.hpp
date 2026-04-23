/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        sds_sensor.hpp
 * Description:  Node for SDS Sensor
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
#include "cg_status.h"


/*

SDS Sensor node to work with the
https://github.com/ARM-software/SDS-Framework

Synchronous Data Stream (SDS) Framework allowing 
to record real-world data off a device and playing 
it back on Arm Virtual Hardware.

*/

namespace arm_cmsis_stream {

template<typename OUT,int outputSize>
class SDSSensor;

template<int outputSize>
class SDSSensor<uint8_t,outputSize>:public NodeBase
{
public:
    SDSSensor(FIFOBase<uint8_t> &dst,
              sds_sensor_cg_connection_t *sdsConnection,
              driftDelegate_t drift_delegate=nullptr,
              void *delegate_data=nullptr):
    mSDS(sdsConnection),
    m_drift_delegate(drift_delegate),
    m_delegate_data(delegate_data),
    mDst(dst){
        /* 
           Amount of data to read from the sensor in the run
           method.

           In synchronous scheduling mode it is
           always outputSize. In asynchronous scheduling
           mode one can read less.


         */
        mToGenerate = outputSize;
    };

    /* This function is only used in asynchronous mode */
    int prepareForRunning() final
    {
        /* What is the maximum amount of samples that can
           be read during the next run.
           This value is limited to outputSize

        */
        mToGenerate = this->nbOfFreeSamplesInFIFO();
        if (mToGenerate>outputSize)
        {
            mToGenerate = outputSize;
        }
        /* If overflow of output FIFO, execution is skipped */
        if (mToGenerate <= 0)
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        /*
           If no data in input SDS stream, we check for
           a cancel event without timeout and then skip 
           the execution
        */
        if (sdsGetCount(mSDS->sdsId)==0)
        {
            uint32_t flags = osThreadFlagsWait(mSDS->cancel_event, osFlagsWaitAny,0);
            if ((flags & osFlagsError) == 0U)
            {
                if (flags & mSDS->cancel_event)
                {
                   return(CG_STOP_SCHEDULER);
                }
            }
            else 
            {
                // osFlagsErrorResource should have been received
                // if the flag has not been set.
                // If it has not been received and the flag has not
                // been set then another error occured.
                if ((flags & osFlagsErrorResource) == 0U)
                {
                    return(CG_OS_ERROR);
                }
            }
            return(CG_SKIP_EXECUTION_ID_CODE);
        }

        /*
           Compute maximum amount of data that need
           to be generated in the output
           (based upon what's available in input and
           free in output)
        */
        uint32_t inSDS = sdsGetCount(mSDS->sdsId) ;
        mToGenerate = mToGenerate > inSDS ? inSDS : mToGenerate ;

        return(CG_SUCCESS);
    };

    int run() final{
        uint8_t *b=this->getWriteBuffer(mToGenerate);

        /* Amount of data that needs to be generated on output */
        uint32_t remaining = mToGenerate;

        while(remaining>0)
        {
            /*

            If no data if available in the SDS stream, we are waiting
            for a timeout or a cancel event

            */
            if (sdsGetCount(mSDS->sdsId)==0)
            {
               uint32_t flags = osThreadFlagsWait(mSDS->cancel_event | mSDS->event, osFlagsWaitAny,mSDS->timeout);
               if ((flags & osFlagsError) != 0U)
               {
                  return(CG_BUFFER_UNDERFLOW);
               }
               if (flags & mSDS->cancel_event)
               {
                  return(CG_STOP_SCHEDULER);
               }
            }
            else
            {
                /*
                
                We check for cancel event without timeout

                */
                uint32_t flags = osThreadFlagsWait(mSDS->cancel_event, osFlagsWaitAny,0);
                if ((flags & osFlagsError) == 0U)
                {
                    if (flags & mSDS->cancel_event)
                    {
                       return(CG_STOP_SCHEDULER);
                    }
                }
                else 
                {
                    // osFlagsErrorResource should have been received
                    // if the flag has not been set.
                    // If it has not been received and the flag has not
                    // been set then another error occured.
                    if ((flags & osFlagsErrorResource) == 0U)
                    {
                        return(CG_OS_ERROR);
                    }
                }
            }
            

            /*
               
               If there is a drift delegate, it is called to make the work.
               Otherwise, sdsRead is used.
               Functions will be called until all the samples
               (remaining) have been generated by sdsRead or driftDelegate
               

            */
            uint32_t num;
            if (m_drift_delegate)
            {
               num=m_drift_delegate(mSDS->sdsId, b, remaining,m_delegate_data);
            }
            else
            {
               num = sdsRead(mSDS->sdsId, b, remaining);
            }
            b += num;
            remaining -= num;
            
        }
        return(CG_SUCCESS);
    };

protected:
    uint8_t * getWriteBuffer(int nb=outputSize){return mDst.getWriteBuffer(nb);};

    bool willOverflow(int nb = outputSize){return mDst.willOverflowWith(nb);};

    int nbOfFreeSamplesInFIFO(){return mDst.nbOfFreeSamplesInFIFO();};
    
    bool p;
    sds_sensor_cg_connection_t *mSDS;
    driftDelegate_t m_drift_delegate;
    void *m_delegate_data;
    uint32_t mToGenerate;
private:
    FIFOBase<uint8_t> &mDst;
};

} // end namespace