#pragma once

#include "GenericNodes.hpp"
#include "StreamNode.hpp"
#include "app_config.hpp"
#include "cg_enums.h"

using namespace arm_cmsis_stream;

// This template can only be instaintiated with OUT = float,
// but we keep it generic for demonstration purposes.
template <typename IN, int inputSamples>
class DebugSink : public GenericSink<IN, inputSamples> {
public:
  enum selector {selMessage=0};
  static std::array<uint16_t,1> selectors;
  DebugSink(FIFOBase<IN> &dst,EventQueue *queue)
      : GenericSink<IN, inputSamples>(dst),ev0(queue) {

        };

  ~DebugSink() {

  };

  int run() final {
    IN *input = this->getReadBuffer();

    ev0.sendAsync(kNormalPriority,selectors[selMessage],input[0]);

    return (CG_SUCCESS);
  };

  void subscribe(int outputPort,StreamNode &dst,int dstPort) final
  {
        ev0.subscribe(dst,dstPort);
  }

protected:
  EventOutput ev0;
};