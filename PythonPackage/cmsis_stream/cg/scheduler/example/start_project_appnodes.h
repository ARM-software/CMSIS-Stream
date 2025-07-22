#ifndef _APPNODES_H_
#define _APPNODES_H_

#include <iostream>

using namespace arm_cmsis_stream;

template<typename IN, int inputSize>
class Sink: public GenericSink<IN, inputSize>
{
public:
    Sink(FIFOBase<IN> &src):
    GenericSink<IN,inputSize>(src){};

    // Used in asynchronous mode. In case of underflow on
    // the input, the execution of this node will be skipped
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

   
    int run() final
    {
        IN *b=this->getReadBuffer();
        (void)b;
        printf("Sink\n");
        // Do something here
        return(CG_SUCCESS);
    };

};


template<typename OUT,int outputSize>
class Source: public GenericSource<OUT,outputSize>
{
public:
    Source(FIFOBase<OUT> &dst):
    GenericSource<OUT,outputSize>(dst){};

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final{
        OUT *b=this->getWriteBuffer();
        (void)b;
        printf("Source\n");
        // Do something here
        return(CG_SUCCESS);
    };

};


template<typename IN, int inputSize,
         typename OUT,int outputSize>
class ProcessingNode:
public GenericNode<IN,inputSize,
                   OUT,outputSize>
{
public:
    /* Constructor needs the input and output FIFOs */
    ProcessingNode(FIFOBase<IN> &src,
                   FIFOBase<OUT> &dst):
    GenericNode<IN,inputSize,
                OUT,outputSize>(src,dst){};

    int prepareForRunning() final
    {
        if (this->willOverflow() ||
            this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };
    
    
    int run() final{
        printf("ProcessingNode\n");
        IN *a=this->getReadBuffer();
        OUT *b=this->getWriteBuffer();
        (void)a;
        (void)b;
        // Do something here
        return(CG_SUCCESS);
    };

};

#endif