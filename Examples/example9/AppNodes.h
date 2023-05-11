/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        AppNodes.h
 * Description:  Application nodes for Example 9
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
#ifndef _APPNODES_H_
#define _APPNODES_H_

#include <iostream>

template<typename IN, int inputSize>
class Sink: public GenericSink<IN, inputSize>
{
public:
    Sink(FIFOBase<IN> &src):GenericSink<IN,inputSize>(src){};

    int prepareForRunning() override
    {
        if (this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run()
    {
        IN *b=this->getReadBuffer();
        printf("Sink\n");
        for(int i=0;i<inputSize;i++)
        {
            std::cout << (int)b[i] << std::endl;
        }
        return(0);
    };

};

template<typename OUT,int outputSize>
class Source: GenericSource<OUT,outputSize>
{
public:
    Source(FIFOBase<OUT> &dst):GenericSource<OUT,outputSize>(dst),mCounter(0){};

    int prepareForRunning() override
    {
        if (this->willOverflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run(){
        OUT *b=this->getWriteBuffer();

        printf("Source\n");
        for(int i=0;i<outputSize;i++)
        {
            b[i] = (OUT)mCounter++;
        }
        return(0);
    };

    int mCounter;

};

template<typename IN1, int inputSize1,
         typename IN2, int inputSize2,
         typename OUT,int outputSize>
class ProcessingNode;

template<int inputSize>
class ProcessingNode<float,inputSize,
                     float,inputSize,
                     float,inputSize>: 
      public GenericNode21<float,inputSize,
                           float,inputSize,
                           float,inputSize>
{
public:
    ProcessingNode(FIFOBase<float> &src1,
                   FIFOBase<float> &src2,
                   FIFOBase<float> &dst):
            GenericNode21<float,inputSize,
                          float,inputSize,
                          float,inputSize>(src1,src2,dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow() ||
            this->willUnderflow1() ||
            this->willUnderflow2()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run(){
        printf("ProcessingNode\n");
        float *a=this->getReadBuffer1();
        float *b=this->getReadBuffer2();
        float *c=this->getWriteBuffer();
        
        for(int i=0;i<inputSize;i++)
        {
            c[i] = a[i] + 0.5f*b[i];
        }
        return(0);
    };

};

#endif