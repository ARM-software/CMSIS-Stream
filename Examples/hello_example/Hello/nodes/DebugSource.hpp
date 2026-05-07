#pragma once



#include "cg_enums.h"
#include "app_config.hpp"
#include "StreamNode.hpp"
#include "GenericNodes.hpp"
#include <atomic>

using namespace arm_cmsis_stream;


template <typename IN, int inputSamples>
class DebugSource: public GenericSource<IN, inputSamples>, public ContextSwitch
{
  public:
   
    DebugSource(FIFOBase<IN> &src,emptySourceParams_t &params)
        : GenericSource<IN, inputSamples>(src), params_(params)
    {

       
    };

    ~DebugSource()
    {
        
    };

    int resume() final
	{
        #if 0
		// Implementation of pause
		if (started_.load() == true) {
			// If it was never started, nothing to do
			return 0;
		}
        osStatus_t err = osTimerStart(params_.hw_.timer_id, params_.ticks);
		if (err != osOK) {
			CMSISSTREAM_LOG_ERR("Timer start failed: %i\n", err);
		}
        else 
        {
		   started_.store(true);
        }
        #endif
		return 0;
	}

    int pause() final
	{
        #if 0
        if (started_.load() == false) {
			// If it was never started, nothing to do
			return 0;
		}
		started_.store(false);
        osStatus_t err = osTimerStop(params_.hw_.timer_id);
		if (err != osOK) {
			CMSISSTREAM_LOG_ERR("Timer stop failed: %i", err);
		}
        #endif
		return 0;
	}


    int run() final
    {
        IN *input = this->getWriteBuffer();

        osDelay(1000); // Simulate some work being done by the node

        std::fill(input, input + inputSamples, params_.val);
        
        return (CG_SUCCESS);
    };

protected:
 std::atomic<bool> started_ = false;
 emptySourceParams_t params_;
  
};