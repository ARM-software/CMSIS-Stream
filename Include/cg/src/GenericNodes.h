/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        GenericNodes.h
 * Description:  C++ support templates for the compute graph with static scheduler
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

#ifndef _SCHEDGEN_H_
#define _SCHEDGEN_H_

#include <vector>
#include <cstring>
#include <stdarg.h>
/* 
Defined in cg_status.h by default but user
may want to use a different header to define the 
error codes of the application
*/
#define CG_SUCCESS_ID_CODE (0)
#define CG_SKIP_EXECUTION_ID_CODE (-5)
#define CG_BUFFER_ERROR_ID_CODE (-6)

/* Node ID is -1 when nodes are not identified for the external
world */
#define UNIDENTIFIED_NODE (-1)

// FIFOS 

#ifdef DEBUGSCHED

#include <iostream>

template<typename T>
struct debugtype{
    typedef T type;
};

template<>
struct debugtype<char>{
    typedef int type;
};

template<typename T>
using Debug = struct debugtype<T>;

#endif

template<typename T>
class FIFOBase{
public:
    virtual T* getWriteBuffer(int nb)=0;
    virtual T* getReadBuffer(int nb)=0;
    virtual ~FIFOBase() {};
    /*

    Below functions are only useful in asynchronous mode.
    Synchronous implementation can provide an empty
    implementation.

    */
    virtual bool willUnderflowWith(int nb) const = 0;
    virtual bool willOverflowWith(int nb) const = 0;
    virtual int nbSamplesInFIFO() const = 0;
    virtual int nbOfFreeSamplesInFIFO() const = 0;

};

template<typename T, int length, int isArray=0, int isAsync = 0>
class FIFO;

/* Real FIFO, Synchronous */
template<typename T, int length>
class FIFO<T,length,0,0>: public FIFOBase<T> 
{
    public:
        FIFO(T *buffer,int delay=0):mBuffer(buffer),readPos(0),writePos(delay) {};
        
        /* Constructor used for memory sharing optimization.
           The buffer is a shared memory wrapper */
        FIFO(void *buffer,int delay=0):mBuffer((T*)buffer),readPos(0),writePos(delay) {};

        bool willUnderflowWith(int nb) const final
        {
            return((writePos - readPos - nb)<0);
        }

        bool willOverflowWith(int nb) const final
        {
            return((writePos - readPos + nb)>length);
        }
        int nbSamplesInFIFO() const final {return (writePos - readPos);};
        int nbOfFreeSamplesInFIFO() const final {return (length - writePos + readPos);};

        T * getWriteBuffer(int nb) final
        {
            
            T *ret;
            if (readPos > 0)
            {
                /* This is re-aligning the read buffer.
                   Aligning buffer is better for vectorized code.
                   But it has an impact since more memcpy are
                   executed than required.
                   This is likely to be not so useful in practice
                   so a future version will optimize the memcpy usage
                   */
                memcpy((void*)mBuffer,(void*)(mBuffer+readPos),(writePos-readPos)*sizeof(T));
                writePos -= readPos;
                readPos = 0;
            }
            
            ret = mBuffer + writePos;
            writePos += nb; 
            return(ret);
        };

        T* getReadBuffer(int nb) final
        {
            
            T *ret = mBuffer + readPos;
            readPos += nb;
            return(ret);
        }

        #ifdef DEBUGSCHED
        void dump()
        {
            int nb=0;
            std::cout << std::endl;
            for(int i=0; i < length ; i++)
            {
                std::cout << (typename Debug<T>::type)mBuffer[i] << " ";
                nb++;
                if (nb == 10)
                {
                    nb=0;
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        #endif

    protected:
        T * const mBuffer;
        int readPos,writePos;
};

/* Buffer, Synchronous */
template<typename T, int length>
class FIFO<T,length,1,0>: public FIFOBase<T> 
{
    public:
        /* No delay argument for this version of the FIFO.
           This version will not be generated when there is a delay
        */
        FIFO(T *buffer):mBuffer(buffer) {};
        FIFO(void *buffer):mBuffer((T*)buffer) {};

        /* 
           Not used in synchronous mode 
           and this version of the FIFO is
           never used in asynchronous mode 
           so empty functions are provided.
        */
        bool willUnderflowWith(int nb) const final {(void)nb;return false;};
        bool willOverflowWith(int nb) const final {(void)nb;return false;};
        int nbSamplesInFIFO() const final {return(0);};
        int nbOfFreeSamplesInFIFO() const final {return( 0);};


        T* getWriteBuffer(int nb) final
        {
            (void)nb;
            return(mBuffer);
        };

        T* getReadBuffer(int nb) final
        {
            (void)nb;
            return(mBuffer);
        }

        #ifdef DEBUGSCHED
        void dump()
        {
            int nb=0;
            std::cout << std::endl;
            for(int i=0; i < length ; i++)
            {
                std::cout << (typename Debug<T>::type)mBuffer[i] << " ";
                nb++;
                if (nb == 10)
                {
                    nb=0;
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        #endif

    protected:
        T * const mBuffer;
};

/* Real FIFO, Asynchronous */
template<typename T, int length>
class FIFO<T,length,0,1>: public FIFOBase<T> 
{
    public:
        FIFO(T *buffer,int delay=0):mBuffer(buffer),readPos(0),writePos(delay) {};
        FIFO(void *buffer,int delay=0):mBuffer((T*)buffer),readPos(0),writePos(delay) {};

        /* 

        Check for overflow must have been done
        before using this function 
        
        */
        T * getWriteBuffer(int nb) final
        {
            
            T *ret;
            if (readPos > 0)
            {
                memcpy((void*)mBuffer,(void*)(mBuffer+readPos),(writePos-readPos)*sizeof(T));
                writePos -= readPos;
                readPos = 0;
            }
            
            ret = mBuffer + writePos;
            writePos += nb; 
            return(ret);
        };

        /* 
        
        Check for undeflow must have been done
        before using this function 
        
        */
        T* getReadBuffer(int nb) final
        {
           
            T *ret = mBuffer + readPos;
            readPos += nb;
            return(ret);
        }

        bool willUnderflowWith(int nb) const final
        {
            return((writePos - readPos - nb)<0);
        }

        bool willOverflowWith(int nb) const final
        {
            return((writePos - readPos + nb)>length);
        }

        int nbSamplesInFIFO() const final {return (writePos - readPos);};
        int nbOfFreeSamplesInFIFO() const final {return (length - writePos + readPos);};


        #ifdef DEBUGSCHED
        void dump()
        {
            int nb=0;
            std::cout << std::endl;
            std::cout << "FIFO nb samples = " << (writePos - readPos) << std::endl;
            for(int i=0; i < length ; i++)
            {
                std::cout << (typename Debug<T>::type)mBuffer[i] << " ";
                nb++;
                if (nb == 10)
                {
                    nb=0;
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        #endif

    protected:
        T * const mBuffer;
        int readPos,writePos;
};

// GENERIC NODES 

class NodeBase
{
public:
    virtual int run()=0;
    virtual int prepareForRunning()=0;
    virtual ~NodeBase() {};
    void setID(int id){mNodeID = id;};
    int nodeID() const {return(mNodeID);};
protected:
    int mNodeID = UNIDENTIFIED_NODE;
};

template<typename IN, int inputSize,typename OUT, int outputSize>
class GenericNode:public NodeBase
{
public:
     GenericNode(FIFOBase<IN> &src,FIFOBase<OUT> &dst):mSrc(src),mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb = outputSize) {return mDst.getWriteBuffer(nb);};
     IN * getReadBuffer(int nb = inputSize) {return mSrc.getReadBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};
     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

private:
    FIFOBase<IN> &mSrc;
    FIFOBase<OUT> &mDst;
};

template<typename IN, int inputSize,
         typename OUT, int outputSize>
class GenericToManyNode:public NodeBase
{
public:
     GenericToManyNode(FIFOBase<IN> &src,
                       std::initializer_list<FIFOBase<OUT>*> dst):mSrc(src),mDstList(dst){};


protected:
     size_t getNbOutputs() const {return(mDstList.size());};

     IN * getReadBuffer(int nb = inputSize) {return mSrc.getReadBuffer(nb);};
     OUT * getWriteBuffer(int id=0,int nb = outputSize) {return mDstList[id]->getWriteBuffer(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};
     bool willOverflow(int id=0,int nb = outputSize) const {return mDstList[id]->willOverflowWith(nb);};

private:
    FIFOBase<IN> &mSrc;
    const std::vector<FIFOBase<OUT>*> mDstList;
};

template<typename IN, int inputSize,
         typename OUT, int outputSize>
class GenericFromManyNode:public NodeBase
{
public:
     GenericFromManyNode(std::initializer_list<FIFOBase<IN>*> src,
                         FIFOBase<OUT> &dst):mSrcList(src),mDst(dst){};


protected:
     size_t getNbInputs() const {return(mSrcList.size());};

     IN  *getReadBuffer(int id=0,int nb = inputSize) {return mSrcList[id]->getReadBuffer(nb);};
     OUT *getWriteBuffer(int nb = outputSize) {return mDst.getWriteBuffer(nb);};

     bool willUnderflow(int id=0,int nb = inputSize) const {return mSrcList[id]->willUnderflowWith(nb);};
     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};

private:
    const std::vector<FIFOBase<IN>*> mSrcList;
    FIFOBase<OUT> &mDst;

};

template<typename IN, int inputSize,
         typename OUT, int outputSize>
class GenericManyToManyNode:public NodeBase
{
public:
     GenericManyToManyNode(std::initializer_list<FIFOBase<IN>*> src,
                           std::initializer_list<FIFOBase<OUT>*> dst):mSrcList(src),mDstList(dst){};

    
protected:
     size_t getNbInputs() const {return(mSrcList.size());};
     size_t getNbOutputs() const {return(mDstList.size());};

     IN  *getReadBuffer(int id=0,int nb = inputSize) {return mSrcList[id]->getReadBuffer(nb);};
     OUT *getWriteBuffer(int id=0,int nb = outputSize) {return mDstList[id]->getWriteBuffer(nb);};

     bool willUnderflow(int id=0,int nb = inputSize) const {return mSrcList[id]->willUnderflowWith(nb);};
     bool willOverflow(int id=0,int nb = outputSize) const {return mDstList[id]->willOverflowWith(nb);};

private:
    const std::vector<FIFOBase<IN>*> mSrcList;
    const std::vector<FIFOBase<OUT>*> mDstList;
};

template<typename IN, int inputSize,typename OUT1, int output1Size,typename OUT2, int output2Size>
class GenericNode12:public NodeBase
{
public:
     GenericNode12(FIFOBase<IN> &src,FIFOBase<OUT1> &dst1,FIFOBase<OUT2> &dst2):mSrc(src),
     mDst1(dst1),mDst2(dst2){};

protected:
     OUT1 * getWriteBuffer1(int nb=output1Size) {return mDst1.getWriteBuffer(nb);};
     OUT2 * getWriteBuffer2(int nb=output2Size) {return mDst2.getWriteBuffer(nb);};
     IN * getReadBuffer(int nb=inputSize) {return mSrc.getReadBuffer(nb);};

     bool willOverflow1(int nb = output1Size) const {return mDst1.willOverflowWith(nb);};
     bool willOverflow2(int nb = output2Size) const {return mDst2.willOverflowWith(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

private:
    FIFOBase<IN> &mSrc;
    FIFOBase<OUT1> &mDst1;
    FIFOBase<OUT2> &mDst2;
};

template<typename IN,   int inputSize,
         typename OUT1, int output1Size,
         typename OUT2, int output2Size,
         typename OUT3, int output3Size>
class GenericNode13:public NodeBase
{
public:
     GenericNode13(FIFOBase<IN> &src,
                   FIFOBase<OUT1> &dst1,
                   FIFOBase<OUT2> &dst2,
                   FIFOBase<OUT3> &dst3
                   ):mSrc(src),
     mDst1(dst1),mDst2(dst2),mDst3(dst3){};

protected:
     OUT1 * getWriteBuffer1(int nb=output1Size) {return mDst1.getWriteBuffer(nb);};
     OUT2 * getWriteBuffer2(int nb=output2Size) {return mDst2.getWriteBuffer(nb);};
     OUT3 * getWriteBuffer3(int nb=output3Size) {return mDst3.getWriteBuffer(nb);};

     IN * getReadBuffer(int nb=inputSize) {return mSrc.getReadBuffer(nb);};

     bool willOverflow1(int nb = output1Size) const {return mDst1.willOverflowWith(nb);};
     bool willOverflow2(int nb = output2Size) const {return mDst2.willOverflowWith(nb);};
     bool willOverflow3(int nb = output3Size) const {return mDst3.willOverflowWith(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

private:
    FIFOBase<IN> &mSrc;
    FIFOBase<OUT1> &mDst1;
    FIFOBase<OUT2> &mDst2;
    FIFOBase<OUT3> &mDst3;

};

template<typename IN1, int input1Size,typename IN2, int input2Size,typename OUT, int outputSize>
class GenericNode21:public NodeBase
{
public:
     GenericNode21(FIFOBase<IN1> &src1,FIFOBase<IN2> &src2,FIFOBase<OUT> &dst):mSrc1(src1),
     mSrc2(src2),
     mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb=outputSize) {return mDst.getWriteBuffer(nb);};
     IN1 * getReadBuffer1(int nb=input1Size) {return mSrc1.getReadBuffer(nb);};
     IN2 * getReadBuffer2(int nb=input2Size) {return mSrc2.getReadBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};
     bool willUnderflow1(int nb = input1Size) const {return mSrc1.willUnderflowWith(nb);};
     bool willUnderflow2(int nb = input2Size) const {return mSrc2.willUnderflowWith(nb);};

private:
    FIFOBase<IN1> &mSrc1;
    FIFOBase<IN2> &mSrc2;
    FIFOBase<OUT> &mDst;
};



template<typename OUT, int outputSize>
class GenericSource:public NodeBase
{
public:
     GenericSource(FIFOBase<OUT> &dst):mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb=outputSize) {return mDst.getWriteBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};

private:
    FIFOBase<OUT> &mDst;
};

template<typename IN,int inputSize>
class GenericSink:public NodeBase
{
public:
     GenericSink(FIFOBase<IN> &src):mSrc(src){};

protected:
     IN * getReadBuffer(int nb=inputSize) {return mSrc.getReadBuffer(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

private:
    FIFOBase<IN> &mSrc;
};


#define REPEAT(N) for(int i=0;i<N;i++)


template<typename IN, int inputSize,
         typename OUT, int outputSize>
class Duplicate;

template<typename IO, int inputOutputSize>
class Duplicate<IO, inputOutputSize,
                IO, inputOutputSize>:
public GenericToManyNode<IO, inputOutputSize,
                         IO, inputOutputSize>
{
public:
    Duplicate(FIFOBase<IO> &src,
              std::initializer_list<FIFOBase<IO>*> dst):
    GenericToManyNode<IO, inputOutputSize,IO, inputOutputSize>(src,dst)
    {
    };

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
        IO *a=this->getReadBuffer();
        
        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           IO *b=this->getWriteBuffer(i);
           memcpy(b,a,sizeof(IO)*inputOutputSize);
        }
        
        return(CG_SUCCESS_ID_CODE);
    };

};


       

#endif
