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
#include "runtime_sched.h"
#include "stream_generated.h"

using namespace arm_cmsis_stream;



template<typename IN>
class GenericRuntimeSink:public NodeBase
{
public:
     explicit GenericRuntimeSink(RuntimeFIFO &src):mSrc(src){};

protected:
     IN * getReadBuffer(int nb) {return mSrc.getReadBuffer<IN>(nb);};

     bool willUnderflow(int nb) const {return mSrc.willUnderflowWith<IN>(nb);};

private:
    RuntimeFIFO &mSrc;
};

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
    // The constructor for the sink is only using
    // the input FIFO (coming from the generated scheduler).
    // This FIFO is passed to the GenericSink contructor.
    // Implementation of this Sink constructor is doing nothing
    Sink(FIFOBase<IN> &src):GenericSink<IN,inputSize>(src){};

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
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
        printf("Sink\n");
        for(int i=0;i<inputSize;i++)
        {
            std::cout << (int)b[i] << std::endl;
        }
        return(0);
    };

};

template<typename IN,typename OUT>
class GenericRuntimeNode:public NodeBase
{
public:
     explicit GenericRuntimeNode(RuntimeFIFO &src,RuntimeFIFO &dst):mSrc(src),mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb ) {return mDst.getWriteBuffer<OUT>(nb);};
     IN * getReadBuffer(int nb ) {return mSrc.getReadBuffer<IN>(nb);};

     bool willOverflow(int nb ) const {return mDst.willOverflowWith<OUT>(nb);};
     bool willUnderflow(int nb ) const {return mSrc.willUnderflowWith<IN>(nb);};

private:
    RuntimeFIFO &mSrc;
    RuntimeFIFO &mDst;
};

template<typename OUT>
class GenericRuntimeSource:public NodeBase
{
public:
     explicit GenericRuntimeSource(RuntimeFIFO &dst):mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb) {return mDst.getWriteBuffer<OUT>(nb);};

     bool willOverflow(int nb) const {return mDst.willOverflowWith<OUT>(nb);};

private:
    RuntimeFIFO &mDst;
};

template<typename IN>
class Sink<IN,RUNTIME>: public GenericRuntimeSink<IN>
{
public:
    // The constructor for the sink is only using
    // the input FIFO (coming from the generated scheduler).
    // This FIFO is passed to the GenericSink contructor.
    // Implementation of this Sink constructor is doing nothing
    Sink(const arm_cmsis_stream::Node &n,
         RuntimeFIFO &src):GenericRuntimeSink<IN>(src),ndesc(n){};
   

    static int runNode(char* obj)
    {
        Sink<IN,RUNTIME> *n = reinterpret_cast<Sink<IN,RUNTIME> *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(char* obj)
    {
        Sink<IN,RUNTIME> *n = reinterpret_cast<Sink<IN,RUNTIME> *>(obj);
        return(n->prepareForRunning());
    }

    static void deleteNode(char* obj)
    {
        Sink<IN,RUNTIME> *n = reinterpret_cast<Sink<IN,RUNTIME> *>(obj);
        delete n;
    }

    static char* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        auto inputs = ndesc->inputs();
        RuntimeFIFO &i = *ctx.fifos[inputs->Get(0)->id()];

        
        Sink<IN,RUNTIME> *node=new Sink<IN,RUNTIME>(*ndesc,i);
        return(reinterpret_cast<char*>(node));
    }

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    int prepareForRunning() final
    {
        auto inputs = ndesc.inputs();
        if (this->willUnderflow(inputs->Get(0)->nb()))
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
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
        printf("Sink\n");

        auto inputs = ndesc.inputs();

        IN *b=this->getReadBuffer(inputs->Get(0)->nb());
        for(int i=0;i<inputs->Get(0)->nb();i++)
        {
            std::cout << (int)b[i] << std::endl;
        }
        return(0);
    };

protected:
    const arm_cmsis_stream::Node &ndesc;

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
    Source(FIFOBase<OUT> &dst):GenericSource<OUT,outputSize>(dst){};

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        OUT *b=this->getWriteBuffer();

        printf("Source\n");
        for(int i=0;i<outputSize;i++)
        {
            b[i] = (OUT)i;
        }
        return(0);
    };

};

template<typename OUT>
class Source<OUT,RUNTIME>: public GenericRuntimeSource<OUT>
{
public:
    Source(const arm_cmsis_stream::Node &n,
           RuntimeFIFO &dst):GenericRuntimeSource<OUT>(dst),ndesc(n){};

    static int runNode(char* obj)
    {
        Source<OUT,RUNTIME> *n = reinterpret_cast<Source<OUT,RUNTIME> *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(char* obj)
    {
        Source<OUT,RUNTIME> *n = reinterpret_cast<Source<OUT,RUNTIME> *>(obj);
        return(n->prepareForRunning());
    }

    static void deleteNode(char* obj)
    {
        Source<OUT,RUNTIME> *n = reinterpret_cast<Source<OUT,RUNTIME> *>(obj);
        delete n;
    }

    static char* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        auto outputs = ndesc->outputs();
        RuntimeFIFO &i = *ctx.fifos[outputs->Get(0)->id()];

        
        Source<OUT,RUNTIME> *node=new Source<OUT,RUNTIME>(*ndesc,i);
        return(reinterpret_cast<char*>(node));
    }

    int prepareForRunning() final
    {
        auto outputs = ndesc.outputs();
        if (this->willOverflow(outputs->Get(0)->nb()))
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final{
        printf("Source\n");
        auto outputs = ndesc.outputs();
        OUT *b=this->getWriteBuffer(outputs->Get(0)->nb());

        for(int i=0;i<outputs->Get(0)->nb();i++)
        {
            b[i] = (OUT)i;
        }

        return(0);
    };
protected:
    const arm_cmsis_stream::Node &ndesc;
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
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(FIFOBase<IN> &src,
                   FIFOBase<IN> &dst):GenericNode<IN,inputOutputSize,
                                                  IN,inputOutputSize>(src,dst){};

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
        IN *a=this->getReadBuffer();
        IN *b=this->getWriteBuffer();
        for(int i=0;i<inputOutputSize;i++)
        {
            b[i] = a[i]+1;
        }
        return(0);
    };

};

template<typename IN>
class ProcessingNode<IN,RUNTIME,
                     IN,RUNTIME>: 
      public GenericRuntimeNode<IN,IN>
{
public:
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(const arm_cmsis_stream::Node &n,
                   RuntimeFIFO &src,
                   RuntimeFIFO &dst):GenericRuntimeNode<IN,IN>(src,dst),ndesc(n){};

    static int runNode(char* obj)
    {
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *n = reinterpret_cast<ProcessingNode<IN,RUNTIME,IN,RUNTIME> *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(char* obj)
    {
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *n = reinterpret_cast<ProcessingNode<IN,RUNTIME,IN,RUNTIME> *>(obj);
        return(n->prepareForRunning());
    }

    static void deleteNode(char* obj)
    {
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *n = reinterpret_cast<ProcessingNode<IN,RUNTIME,IN,RUNTIME> *>(obj);
        delete n;
    }

    static char* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        
        auto inputs = ndesc->inputs();
        RuntimeFIFO &i = *ctx.fifos[inputs->Get(0)->id()];

        auto outputs = ndesc->outputs();
        RuntimeFIFO &o = *ctx.fifos[outputs->Get(0)->id()];

        
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *node=new ProcessingNode<IN,RUNTIME,IN,RUNTIME>(*ndesc,i,o);
        return(reinterpret_cast<char*>(node));
    }

    /* In asynchronous mode, node execution will be 
       skipped in case of underflow on the input 
       or overflow in the output.
    */
    int prepareForRunning() final
    {
        auto inputs = ndesc.inputs();
        auto outputs = ndesc.outputs();
        if (this->willOverflow(inputs->Get(0)->nb()) ||
            this->willUnderflow(outputs->Get(0)->nb()))
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

        auto inputs = ndesc.inputs();
        auto outputs = ndesc.outputs();

        IN *a=this->getReadBuffer(inputs->Get(0)->nb());
        IN *b=this->getWriteBuffer(outputs->Get(0)->nb());
        for(int i=0;i<inputs->Get(0)->nb();i++)
        {
            b[i] = a[i]+1;
        }
        return(0);
    };
protected:
    const arm_cmsis_stream::Node &ndesc;
};


#endif