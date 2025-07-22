/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        AppNodes.h
 * Description:  Application nodes for Example 1
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
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
#include "cg_enums.h"

using namespace arm_cmsis_stream;


template<typename IN, int inputSize>
class BufferSink: public GenericSink<IN, inputSize>
{
public:
    BufferSink(FIFOBase<IN> &src):GenericSink<IN,inputSize>(src){};

    int prepareForRunning() override
    {
        if (this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run()
    {
        IN *b=this->getReadBuffer();
        printf("BufferSink\n");
        
        return(CG_SUCCESS);
    };

};

template<typename OUT,int outputSize>
class BufferSource: GenericSource<OUT,outputSize>
{
public:
    BufferSource(FIFOBase<OUT> &dst):GenericSource<OUT,outputSize>(dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run(){
        OUT *b=this->getWriteBuffer();

        printf("BufferSource\n");
       
        return(CG_SUCCESS);
    };


};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class InPlace;

template<typename IN>
class InPlace<std::shared_ptr<IN>,1,std::shared_ptr<IN>,1>: 
public GenericNode<std::shared_ptr<IN>,1,std::shared_ptr<IN>,1>
{
public:
    InPlace(FIFOBase<std::shared_ptr<IN>> &src,
            FIFOBase<std::shared_ptr<IN>> &dst):
    GenericNode<std::shared_ptr<IN>,1,std::shared_ptr<IN>,1>(src,dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    int run(){
        printf("InPlace\n");
        std::shared_ptr<IN> *a=this->getReadBuffer();
        std::shared_ptr<IN> *b=this->getWriteBuffer();
        
        return(CG_SUCCESS);
    };

};

/*

As an example we specify the constraint that the InPlace node can
only be used with a buffer that is not shared

*/
template<typename IN>
class InPlace<Shared<IN,false>,1,
              Shared<IN,false>,1>: 
public GenericNode<Shared<IN,false>,1,
                   Shared<IN,false>,1>
{
public:
    InPlace(FIFOBase<Shared<IN,false>> &src,
            FIFOBase<Shared<IN,false>> &dst):
    GenericNode<Shared<IN,false>,1,Shared<IN,false>,1>(src,dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    int run(){
        printf("InPlace\n");
        Shared<IN,false> *a=this->getReadBuffer();
        Shared<IN,false> *b=this->getWriteBuffer();
        
        return(CG_SUCCESS);
    };

};

template<typename IN, int inputSize,
         typename OUT,int outputSize>
class BufferCopy;

template<typename IN,
         typename OUT>
class BufferCopy<IN,1,OUT,1>: 
public GenericNode<IN,1,OUT,1>
{
public:
    BufferCopy(FIFOBase<IN> &src,
            FIFOBase<OUT> &dst):
    GenericNode<IN,1,OUT,1>(src,dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow() ||
            this->willUnderflow()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    int run(){
        printf("BufferCopy\n");
        IN *a=this->getReadBuffer();
        OUT *b=this->getWriteBuffer();
        
        return(CG_SUCCESS);
    };

};

template<typename IN1, int inputSize1,
         typename IN2, int inputSize2,
         typename OUT,int outputSize>
class Processing;

/*

As an example we specify the constraint that the Processing 
node is only reading its second argument : so it can have any
reference count.
The first argument is a modifed buffer so must not be shared

*/
template<typename IN,bool shared>
class Processing<Shared<IN,false>,1,
                 Shared<IN,shared>,1,
                 Shared<IN,false>,1>: 
public GenericNode21<Shared<IN,false>,1,
                     Shared<IN,shared>,1,
                     Shared<IN,false>,1>
{
public:
    Processing(FIFOBase<Shared<IN,false>> &newBuf,
               FIFOBase<Shared<IN,shared>> &src,
               FIFOBase<Shared<IN,false>> &dst):
    GenericNode21<Shared<IN,false>,1,
                  Shared<IN,shared>,1,
                  Shared<IN,false>,1>(newBuf,src,dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow() ||
            this->willUnderflow1() ||
            this->willUnderflow2()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    int run(){
        printf("Processing\n");
        Shared<IN,false> *newBuf=this->getReadBuffer1();
        Shared<IN,shared> *a=this->getReadBuffer2();
        Shared<IN,false> *b=this->getWriteBuffer();
        
        return(CG_SUCCESS);
    };

};

template<typename IN>
class Processing<std::shared_ptr<IN>,1,
                 std::shared_ptr<IN>,1,
                 std::shared_ptr<IN>,1>: 
public GenericNode21<std::shared_ptr<IN>,1,
                     std::shared_ptr<IN>,1,
                     std::shared_ptr<IN>,1>
{
public:
    Processing(FIFOBase<std::shared_ptr<IN>> &newBuf,
               FIFOBase<std::shared_ptr<IN>> &src,
               FIFOBase<std::shared_ptr<IN>> &dst):
    GenericNode21<std::shared_ptr<IN>,1,
                  std::shared_ptr<IN>,1,
                  std::shared_ptr<IN>,1>(newBuf,src,dst){};

    int prepareForRunning() override
    {
        if (this->willOverflow() ||
            this->willUnderflow1() ||
            this->willUnderflow2()
           )
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    int run(){
        printf("Processing\n");
        std::shared_ptr<IN> *newBuf=this->getReadBuffer1();
        std::shared_ptr<IN> *a=this->getReadBuffer2();
        std::shared_ptr<IN> *b=this->getWriteBuffer();
        
        return(CG_SUCCESS);
    };

};

#endif