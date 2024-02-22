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
     explicit GenericRuntimeSink(const arm_cmsis_stream::Node &n,
                                 RuntimeEdge &src):ndesc(n),mSrc(src){};

     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};

protected:
     IN * getReadBuffer(const int nb=0) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};

     bool willUnderflow(const int nb=0) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
};

template<typename IN,typename OUT>
class GenericRuntimeToManyNode:public NodeBase
{
public:
     explicit GenericRuntimeToManyNode(const arm_cmsis_stream::Node &n,
                                       RuntimeEdge &src,
                                       std::vector<RuntimeEdge*> dst):ndesc(n),mSrc(src),mDstList(dst){};

     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};
     std::size_t nb_output_samples(const int i) const {return(ndesc.outputs()->Get(i)->nb());};

protected:
     size_t getNbOutputs() const {return(mDstList.size());};

     IN * getReadBuffer(const int nb=0) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};
     OUT * getWriteBuffer(const int io_id,
                          const int nb=0) {return (OUT*)mDstList[io_id]->getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),io_id,nb);};

     bool willUnderflow(const int nb=0) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};
     bool willOverflow(const int io_id,
                       const int nb=0) const {return mDstList[io_id]->willOverflowWith(*(ndesc.outputs()),sizeof(OUT),io_id,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
    const std::vector<RuntimeEdge*> mDstList;
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
     explicit GenericRuntimeNode(const arm_cmsis_stream::Node &n,
                                 RuntimeEdge &src,
                                 RuntimeEdge &dst):ndesc(n),mSrc(src),mDst(dst){};

     std::size_t nb_input_samples() const {return(ndesc.inputs()->Get(0)->nb());};
     std::size_t nb_output_samples() const {return(ndesc.outputs()->Get(0)->nb());};


protected:
     OUT * getWriteBuffer(const int nb=0 ) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     IN * getReadBuffer(const int nb=0 ) {return (IN*)mSrc.getReadBuffer(*(ndesc.inputs()),sizeof(IN),0,nb);};

     bool willOverflow(const int nb=0 ) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};
     bool willUnderflow(const int nb=0 ) const {return mSrc.willUnderflowWith(*(ndesc.inputs()),sizeof(IN),0,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mSrc;
    RuntimeEdge &mDst;
};

template<typename OUT>
class GenericRuntimeSource:public NodeBase
{
public:
     explicit GenericRuntimeSource(const arm_cmsis_stream::Node &n,
                                   RuntimeEdge &dst):ndesc(n),mDst(dst){};

     std::size_t nb_output_samples() const {return(ndesc.outputs()->Get(0)->nb());};

protected:
     OUT * getWriteBuffer(const int nb=0) {return (OUT*)mDst.getWriteBuffer(*(ndesc.outputs()),sizeof(OUT),0,nb);};

     bool willOverflow(const int nb=0) const {return mDst.willOverflowWith(*(ndesc.outputs()),sizeof(OUT),0,nb);};

private:
    const arm_cmsis_stream::Node &ndesc;
    RuntimeEdge &mDst;
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
         RuntimeEdge &src):GenericRuntimeSink<IN>(n,src){};
   


    static int runNode(NodeBase* obj)
    {
        Sink<IN,RUNTIME> *n = reinterpret_cast<Sink<IN,RUNTIME> *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(NodeBase* obj)
    {
        Sink<IN,RUNTIME> *n = reinterpret_cast<Sink<IN,RUNTIME> *>(obj);
        return(n->prepareForRunning());
    }

    static NodeBase* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        auto inputs = ndesc->inputs();
        RuntimeEdge &i = *ctx.fifos[inputs->Get(0)->id()];

        
        Sink<IN,RUNTIME> *node=new Sink<IN,RUNTIME>(*ndesc,i);
        return(static_cast<NodeBase*>(node));
    }

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
        printf("Sink\n");


        IN *b=this->getReadBuffer();
        for(int i=0;i<this->nb_input_samples();i++)
        {
            std::cout << (int)b[i] << std::endl;
        }
        return(0);
    };


};


template<>
class Duplicate<char, RUNTIME,char, RUNTIME>:
public GenericRuntimeToManyNode<char,char>
{
public:
    using DUP = Duplicate<char,RUNTIME,char,RUNTIME>;

    explicit Duplicate(const arm_cmsis_stream::Node &n,
              RuntimeEdge &src,
              std::vector<RuntimeEdge*> dst):
    GenericRuntimeToManyNode<char,char>(n,src,dst)
    {
    };


    static int runNode(NodeBase* obj)
    {
        DUP *n = reinterpret_cast<DUP *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(NodeBase* obj)
    {
        DUP *n = reinterpret_cast<DUP *>(obj);
        return(n->prepareForRunning());
    }

    static NodeBase* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        auto inputs = ndesc->inputs();
        auto outputs = ndesc->outputs();

        RuntimeEdge &i = *ctx.fifos[inputs->Get(0)->id()];
        std::vector<RuntimeEdge*> o;

        for(auto out:*outputs)
        {
            o.push_back(ctx.fifos[out->id()].get());
        }
        
        DUP *node=new DUP(*ndesc,i,o);
        return(static_cast<NodeBase*>(node));
    }

    int prepareForRunning() final
    {

        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           if (this->willOverflow(i))
           {
              return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
           }
        }


        return(CG_SUCCESS_ID_CODE);
    };

    int run() final {

        char *a=this->getReadBuffer();
        
        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           char *b=this->getWriteBuffer(i);
           memcpy(b,a,this->nb_input_samples());
        }
        
        return(CG_SUCCESS_ID_CODE);
    };

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
           RuntimeEdge &dst):GenericRuntimeSource<OUT>(n,dst){};


    static int runNode(NodeBase* obj)
    {
        Source<OUT,RUNTIME> *n = reinterpret_cast<Source<OUT,RUNTIME> *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(NodeBase* obj)
    {
        Source<OUT,RUNTIME> *n = reinterpret_cast<Source<OUT,RUNTIME> *>(obj);
        return(n->prepareForRunning());
    }

    static NodeBase* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        auto outputs = ndesc->outputs();
        RuntimeEdge &i = *ctx.fifos[outputs->Get(0)->id()];

        
        Source<OUT,RUNTIME> *node=new Source<OUT,RUNTIME>(*ndesc,i);
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
        OUT *b=this->getWriteBuffer();

        for(int i=0;i<this->nb_output_samples();i++)
        {
            b[i] = (OUT)i;
        }

        return(0);
    };

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
                   RuntimeEdge &src,
                   RuntimeEdge &dst,
                   const uint32_t inc):GenericRuntimeNode<IN,IN>(n,src,dst),mInc(inc){};


    static int runNode(NodeBase* obj)
    {
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *n = reinterpret_cast<ProcessingNode<IN,RUNTIME,IN,RUNTIME> *>(obj);
        return(n->run());
    }

    static int prepareForRunningNode(NodeBase* obj)
    {
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *n = reinterpret_cast<ProcessingNode<IN,RUNTIME,IN,RUNTIME> *>(obj);
        return(n->prepareForRunning());
    }


    static NodeBase* mkNode(const runtime_context &ctx, 
                        const arm_cmsis_stream::Node *ndesc)
    {
        
        auto inputs = ndesc->inputs();
        RuntimeEdge &i = *ctx.fifos[inputs->Get(0)->id()];

        auto outputs = ndesc->outputs();
        RuntimeEdge &o = *ctx.fifos[outputs->Get(0)->id()];

        // Extract values from data
        const int8_t *d = ndesc->node_data()->data();
        const int32_t *v = reinterpret_cast<const int32_t*>(d);


        
        ProcessingNode<IN,RUNTIME,IN,RUNTIME> *node=new ProcessingNode<IN,RUNTIME,IN,RUNTIME>(*ndesc,i,o,*v);
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


        IN *a=this->getReadBuffer();
        IN *b=this->getWriteBuffer();
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


#endif