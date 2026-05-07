#pragma once



#include "cg_enums.h"
#include "app_config.hpp"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"


using namespace arm_cmsis_stream;

// This template can only be instaintiated with OUT = float, 
//but we keep it generic for demonstration purposes.
template <typename OUT, int outputSamples>
class DebugSink: public GenericSink<OUT, outputSamples>
{
  public:
   
    DebugSink(FIFOBase<OUT> &dst)
        : GenericSink<OUT, outputSamples>(dst)
    {

       
    };

    ~DebugSink()
    {
        
    };


    int run() final
    {
        OUT *input = this->getReadBuffer();

        float tmp;
        for (int i = 0; i < outputSamples; i++)
        {
            tmp += input[i];
        }
        printf("[%d] Temp = %f\n", counter++, tmp);
        
        return (CG_SUCCESS);
    };
protected:
 uint32_t counter = 0;
  
};