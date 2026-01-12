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

#include <chrono>
#include <thread>
#include <iostream>
#include "host_com.hpp"
#include <variant>
#include <stdlib.h>

using namespace arm_cmsis_stream;

/*

C++ template for the Sink node.
A generic implementation is provided
for all types IN.

The template has two arguments: 
- The input datatype IN
- The number of consumed sample on this input : inputSize

*/
template<typename IN, int inputSize>
class Sink: public GenericSink<IN, inputSize>
{
public:
    enum selector {selIncrement=0};
    static std::array<uint16_t,1> selectors;

    // The constructor for the sink is only using
    // the input FIFO (coming from the generated scheduler).
    // This FIFO is passed to the GenericSink contructor.
    // Implementation of this Sink constructor is doing nothing
    Sink(FIFOBase<IN> &src,EventQueue *evtQueue):GenericSink<IN,inputSize>(src),
    eventCount(0),
    mustStop(false),
    increment(1),
    queue_(evtQueue)
    {};

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION); // Skip execution
        }

        return(0);
    };

    // Implementation of the node.
    // The input is printed on stdout.
    // So this node will build only if the IN
    // datatype has an implementation of << to
    // be printed on stdout
    int run() final
    {
        IN *b=this->getReadBuffer();
        #if !defined(CG_HOST)
        printf("Sink\n");
        #endif
        //for(int i=0;i<inputSize;i++)
        //{
        //    std::cout << (int)b[i] << std::endl;
        //}
        if (mustStop)
        {
            return(CG_STOP_SCHEDULER); // Stop the graph
        }
        return(0);
    };

    void setValue(int32_t v)
    {
        value = v;
    }

    void processEvent(int dstPort,Event &&evt) final
    {
        std::cout << evt.event_id << std::endl;
        if (evt.event_id == kStopGraph)
        {
            mustStop = true; // Stop the graph
            return;
        }
        if ((evt.event_id != kValue) && (evt.event_id != kDo))
        {
            return;
        }
        eventCount++;

        if (evt.event_id == kValue)
        {
            if (evt.wellFormed<int32_t>())
            {
                evt.apply<int32_t>(&Sink::setValue, *this);
            }
            
        }

        if (evt.event_id == kDo)
        {
            std::cout << "Sink value = " << value << std::endl;
        }

        Event newEvt(kDo,kNormalPriority,1.0f,2.0f);
        queue_->push(DistantDestination{this->nodeID()},std::move(newEvt));

        std::cout << "Sink received direct event on port " << dstPort << ": evt id " 
               << evt.event_id << " val = " << eventCount << "  " << std::endl;
 
    }



protected:

   bool mustStop;
   int eventCount;
   int increment;
   uint32_t value=0;
   EventQueue *queue_;
};

/*

Source template.
It is very similar to the Sink but inputs have been
replaced by outputs.

*/
template<typename OUT,int outputSize>
class Source: public GenericSource<OUT,outputSize>
{
public:
    enum selector {selIncrement=0};
    static std::array<uint16_t,1> selectors;


    Source(FIFOBase<OUT> &dst,EventQueue *queue):GenericSource<OUT,outputSize>(dst),ev0(queue)
    {
    };

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION); // Skip execution
        }

        return(0);
    };

    int run() final{
        OUT *b=this->getWriteBuffer();
        #if !defined(CG_HOST)
        printf("Source\n");
        #endif
        int r = rand();
        r = 50 + (r % 25);
        std::this_thread::sleep_for(std::chrono::milliseconds(r));

        for(int i=0;i<outputSize;i++)
        {
            b[i] = (OUT)i;
        }

#if !defined(CG_HOST)
        ev0.sendAsync(kNormalPriority,kDo); // Send a async Do event to all subscribers
        ev0.sendAsync(kNormalPriority,selectors[selIncrement],1,1.0,-4);
#endif
        return(0);
    };

    void subscribe(int outputPort,StreamNode &dst,int dstPort) final
    {
        ev0.subscribe(dst,dstPort);
    }

protected:
   EventOutput ev0;

};

/* 

Definition of the Generic ProcessingNode template
defining one input and one output.
The generic template is not implemented.
Instead, a specialized implementation is provided after.

*/
template<typename IN, int inputSize,
         typename OUT,int outputSize>
class ProcessingNode;


/*

Specialized implementation of the template with the constraints:
IN == OUT
inputSize == outputSize == inputOutputSize

If the Python is generating a node where those constraints are
not respected, the C++ will not typecheck and build.

There are a remaining degrees of freedom : 
The datatype (for input and output) can be chosen : IN
The number of samples (produced and consumed) 
can be chosen : inputOutputSize

As consequence, the template specialization still use some
arguments IN and inputOutputSize.

*/
template<typename IN, int inputOutputSize>
class ProcessingNode<IN,inputOutputSize,
                     IN,inputOutputSize>: 
      public GenericNode<IN,inputOutputSize,
                         IN,inputOutputSize>
{
public:
    enum selector {selReset=0,
                   selIncrement=1};
    static std::array<uint16_t,2> selectors;

    /* Constructor needs the input and output FIFOs */
    ProcessingNode(FIFOBase<IN> &src,
                   FIFOBase<IN> &dst):GenericNode<IN,inputOutputSize,
                                                  IN,inputOutputSize>(src,dst)
    {
    };

    /* In asynchronous mode, node execution will be 
       skipped in case of underflow on the input 
       or overflow in the output.
    */
    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION); // Skip execution
        }

        return(0);
    };
    
    /* 
       Node processing
       1 is added to the input
    */
    int run() final{
        #if !defined(CG_HOST)
        printf("ProcessingNode\n");
        #else
        UniquePtr<float> ptr(10);
       
        for(int i=0;i<10;i++)
        {
            ptr[i] = (float)i;
        }
        TensorPtr<float> t = TensorPtr<float>::create_with((uint8_t)1,
                                                           cg_tensor_dims_t{10},
                                                           std::move(ptr));
                                               
        Host::send(this->nodeID(),kValue,std::move(t));
        Host::send(this->nodeID(),kValue,int32_t(42),int16_t(4));
        #endif
        IN *a=this->getReadBuffer();
        IN *b=this->getWriteBuffer();
        for(int i=0;i<inputOutputSize;i++)
        {
            b[i] = a[i]+1;
        }
        return(0);
    };

};


class EvtSource:public StreamNode
{
public:
    static std::array<uint16_t,1> selectors;

    EvtSource(EventQueue *queue):StreamNode(),ev0(queue){};

    void subscribe(int outputPort,StreamNode &dst,int dstPort) final
    {
        ev0.subscribe(dst,dstPort);
    }

protected:
   EventOutput ev0;
};

class EvtSink: public StreamNode
{
public:
    static std::array<uint16_t,2> selectors;

    // The constructor for the sink is only using
    // the input FIFO (coming from the generated scheduler).
    // This FIFO is passed to the GenericSink contructor.
    // Implementation of this Sink constructor is doing nothing
    EvtSink(EventQueue *queue):StreamNode(),eventCount(0),queue_(queue){};

    void processEvent(int dstPort,Event &&evt) final
    {
        eventCount++;
        
        Event newEvt(kDo,kNormalPriority,1.0f,2.0f);
        queue_->push(DistantDestination{this->nodeID()},std::move(newEvt));

        // Display the event received from another node
        std::cout << "Sink received event on port " << dstPort << ": evt id " 
                  << evt.event_id << " val = " << eventCount << "  " << std::endl;
    }

protected:
   int eventCount;
   EventQueue *queue_;

};

#endif