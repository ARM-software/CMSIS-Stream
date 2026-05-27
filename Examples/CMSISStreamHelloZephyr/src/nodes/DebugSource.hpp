#pragma once

#include "GenericNodes.hpp"
#include "StreamNode.hpp"
#include "app_config.hpp"
#include "cg_enums.h"
#include <atomic>


using namespace arm_cmsis_stream;

template <typename IN, int inputSamples>
class DebugSource : public GenericSource<IN, inputSamples>,
                    public ContextSwitch {
public:
  DebugSource(FIFOBase<IN> &src, emptySourceParams_t &params)
      : GenericSource<IN, inputSamples>(src), params_(params) {

        };

  ~DebugSource() {

  };

  int resume() final {
    // Implementation of pause
    if (started_.load() == true) {
      // If it was never started, nothing to do
      return 0;
    }
    if (params_.hw_.timer != nullptr) {
      k_timer_start(params_.hw_.timer, K_MSEC(params_.ticks), K_MSEC(params_.ticks));
    }
    started_.store(true);
    return 0;
  }

  int pause() final {
    if (started_.load() == false) {
      // If it was never started, nothing to do
      return 0;
    }
    started_.store(false);
    if (params_.hw_.timer != nullptr) {
      k_timer_stop(params_.hw_.timer);
    }
    return 0;
  }

  int run() final {
    IN *input = this->getWriteBuffer();

    k_sleep(K_MSEC(1000)); // Simulate some work being done by the node

    std::fill(input, input + inputSamples, params_.val);

    return (CG_SUCCESS);
  };

protected:
  std::atomic<bool> started_ = false;
  emptySourceParams_t params_;
};
