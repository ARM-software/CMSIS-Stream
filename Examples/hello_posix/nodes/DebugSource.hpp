#pragma once

#include "GenericNodes.hpp"
#include "StreamNode.hpp"
#include "app_config.hpp"
#include "cg_enums.h"

#include <atomic>
#include <chrono>
#include <thread>

using namespace arm_cmsis_stream;

template <typename IN, int inputSamples>
class DebugSource : public GenericSource<IN, inputSamples>,
                    public ContextSwitch {
  public:
    DebugSource(FIFOBase<IN> &src, emptySourceParams_t &params)
        : GenericSource<IN, inputSamples>(src), params_(params)
    {
    }

    int resume() final
    {
        started_.store(true);
        return 0;
    }

    int pause() final
    {
        started_.store(false);
        return 0;
    }

    int run() final
    {
        IN *input = this->getWriteBuffer();

        std::this_thread::sleep_for(std::chrono::milliseconds(params_.hw_.delay_ms));
        std::fill(input, input + inputSamples, params_.val);

        return CG_SUCCESS;
    }

  protected:
    std::atomic<bool> started_ = false;
    emptySourceParams_t params_;
};
