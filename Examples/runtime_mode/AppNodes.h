/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        AppNodes.h
 * Description:  Application nodes for Example simple
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
#include "GenericRuntimeNodes.h"
#include "stream_generated.h"

using namespace arm_cmsis_stream;


/**
 * @brief      This class describes a sink.
 */
class Sink: public GenericRuntimeSink<float>
{
public:
    /**
     * @brief      Constructs a new instance.
     *
     * @param[in]  n     flatbuffer description of this node instance
     * @param      src   The source FIFO
     */
    Sink(const Node &n,
         RuntimeEdge &src):GenericRuntimeSink<float>(n,src){};
   
    /**
     * UUID identifying this class
     */
    constexpr static std::array<uint8_t,16> uuid    = {0xc3,0x0e,0xa9,0xea,0xe9,0xc3,0x46,0x38,0xbb,0xc6,0x02,0x1f,0xa3,0x54,0x9d,0x93};

    /**
     * @brief      Make a new sink instance
     *
     * @param[in]  ctx    The runtime context
     * @param[in]  ndesc  The flatbuffer description of the instance
     *
     * @return     Pointer to the newly created node.
     */
    static NodeBase* mkNode(const RuntimeContext &ctx, 
                            const Node *ndesc)
    {
        auto inputs = ndesc->inputs();
        RuntimeEdge &i = *ctx.fifos[inputs->Get(0)->id()];
    
        Sink *node=new Sink(*ndesc,i);
        return(static_cast<NodeBase*>(node));
    }

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    // 
    
    /**
     * @brief      Check if node can be run in asynchronous mode
     *
     * @return     Error code
     */
    int prepareForRunning() final
    {
        // check if FIFO would underflow using default value
        // (nb samples got from flatbuffer description)
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

   
    /**
     * @brief      Run the node
     *
     * @return     Error code
     */
    int run() final
    {
        printf("Sink\n");


        float *b=this->getReadBuffer();
        for(int i=0;i<this->nb_input_samples();i++)
        {
            std::cout << (int)b[i] << std::endl;
        }
        return(0);
    };


};




class Source: public GenericRuntimeSource<float>
{
public:
    Source(const Node &n,
           RuntimeEdge &dst):GenericRuntimeSource<float>(n,dst){};

    constexpr static std::array<uint8_t,16> uuid    = {0xc0,0x08,0x9f,0x59,0x2f,0x33,0x4e,0xc4,0x90,0x23,0x30,0xf6,0x9f,0x0f,0x48,0x33};


    static NodeBase* mkNode(const RuntimeContext &ctx, 
                            const Node *ndesc)
    {
        auto outputs = ndesc->outputs();
        RuntimeEdge &i = *ctx.fifos[outputs->Get(0)->id()];

        
        Source *node=new Source(*ndesc,i);
        return(static_cast<NodeBase*>(node));
    }

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        printf("Source\n");
        float *b=this->getWriteBuffer();

        for(int i=0;i<this->nb_output_samples();i++)
        {
            b[i] = (float)i;
        }

        return(0);
    };

};



class ProcessingNode: public GenericRuntimeNode<float,float>
{
public:
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(const Node &n,
                   RuntimeEdge &src,
                   RuntimeEdge &dst,
                   const uint32_t inc):GenericRuntimeNode<float,float>(n,src,dst),mInc(inc){};

    constexpr static std::array<uint8_t,16> uuid   = {0x3f,0xf6,0x2b,0x0c,0x9a,0xd8,0x44,0x5d,0xbb,0xe9,0x20,0x8d,0x87,0x42,0x34,0x46};


    static NodeBase* mkNode(const RuntimeContext &ctx, 
                            const Node *ndesc)
    {
        
        auto inputs = ndesc->inputs();
        RuntimeEdge &i = *ctx.fifos[inputs->Get(0)->id()];

        auto outputs = ndesc->outputs();
        RuntimeEdge &o = *ctx.fifos[outputs->Get(0)->id()];

        // Extract values from data
        const int8_t *d = ndesc->node_data()->data();
        const int32_t *v = reinterpret_cast<const int32_t*>(d);


        
        ProcessingNode *node=new ProcessingNode(*ndesc,i,o,*v);
        return(static_cast<NodeBase*>(node));
    }

    /* In asynchronous mode, node execution will be 
       skipped in case of underflow on the input 
       or overflow in the output.
    */
    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };
    
    /* 
       Node processing
       1 is added to the input
    */
    int run() final{
        printf("ProcessingNode\n");


        float *a=this->getReadBuffer();
        float *b=this->getWriteBuffer();
        for(int i=0;i<this->nb_input_samples();i++)
        {
            b[i] = a[i]+mInc;
        }
        return(0);
    };

    uint32_t inc(void) const {return mInc;};

protected:
    const uint32_t mInc;
};



class AdderNode: public GenericRuntimeNode21<float,float,float>
{
public:


    /* Constructor needs the input and output FIFOs */
    AdderNode(const Node &n,
              RuntimeEdge &src1,
              RuntimeEdge &src2,
              RuntimeEdge &dst):GenericRuntimeNode21<float,float,float>(n,src1,src2,dst){};

    constexpr static std::array<uint8_t,16> uuid  = {0x6a,0x73,0x38,0x1c,0xcd,0x11,0x4f,0x13,0xba,0x96,0x34,0x75,0x7c,0x2c,0x4a,0x59};


    static NodeBase* mkNode(const RuntimeContext &ctx, 
                            const Node *ndesc)
    {
        
        auto inputs = ndesc->inputs();
        RuntimeEdge &ia = *ctx.fifos[inputs->Get(0)->id()];
        RuntimeEdge &ib = *ctx.fifos[inputs->Get(1)->id()];

        auto outputs = ndesc->outputs();
        RuntimeEdge &o = *ctx.fifos[outputs->Get(0)->id()];

      
        AdderNode *node=new AdderNode(*ndesc,ia,ib,o);
        return(static_cast<NodeBase*>(node));
    }

    /* In asynchronous mode, node execution will be 
       skipped in case of underflow on the input 
       or overflow in the output.
    */
    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow1() ||
            this->willUnderflow2())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };
    
    /* 
       Node processing
       1 is added to the input
    */
    int run() final{
        //printf("AdderNode\n");


        float *a=this->getReadBuffer1();
        float *b=this->getReadBuffer2();
        float *c=this->getWriteBuffer();
        for(int i=0;i<this->nb_input_samples1();i++)
        {
            c[i] = a[i]+b[i];
        }
        return(0);
    };

};


#endif