#pragma once



#include "cg_enums.h"
#include "app_config.hpp"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"


using namespace arm_cmsis_stream;


template <typename OUT, int outputSamples>
class NullSink: public GenericSink<OUT, outputSamples>
{
  public:
   
    NullSink(FIFOBase<OUT> &dst)
        : GenericSink<OUT, outputSamples>(dst)
    {

       
    };

    ~NullSink()
    {
        
    };


    int run() final
    {
        OUT *input = this->getReadBuffer();
        
        return (CG_SUCCESS);
    };

  
};