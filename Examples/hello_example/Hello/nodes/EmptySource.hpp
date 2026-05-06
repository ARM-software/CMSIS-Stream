#pragma once



#include "cg_enums.h"
#include "app_config.hpp"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"


using namespace arm_cmsis_stream;


template <typename IN, int inputSamples>
class EmptySource: public GenericSource<IN, inputSamples>
{
  public:
   
    EmptySource(FIFOBase<IN> &src,emptySourceParams_t &params)
        : GenericSource<IN, inputSamples>(src), params_(params)
    {

       
    };

    ~EmptySource()
    {
        
    };


    int run() final
    {
        IN *input = this->getWriteBuffer();
        
        return (CG_SUCCESS);
    };
protected:
 emptySourceParams_t params_;
  
};