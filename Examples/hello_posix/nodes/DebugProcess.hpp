#pragma once



#include "cg_enums.h"
#include "app_config.hpp"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"


using namespace arm_cmsis_stream;

// This template can only be instantiated with OUT = float, 
//but we keep it generic for demonstration purposes.
template <typename IN, int inputSamples,typename OUT,int outputSamples>
class DebugProcess;

template <typename IN, int inputSamples>
class DebugProcess<IN,inputSamples,IN,1>: public GenericNode<IN, inputSamples,IN,1>
{
  public:
   
    DebugProcess(FIFOBase<IN> &src, FIFOBase<IN> &dst)
        : GenericNode<IN, inputSamples,IN,1>(src,dst)
    {

       
    };

    ~DebugProcess()
    {
        
    };


    int run() final
    {
        IN *input = this->getReadBuffer();
        IN *output = this->getWriteBuffer();

        float tmp = 0.0f;
        for (int i = 0; i < inputSamples; i++)
        {
            tmp += input[i];
        }

        output[0] = tmp;
        
        return (CG_SUCCESS);
    };
  
};