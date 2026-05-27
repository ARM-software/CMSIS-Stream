#pragma once

#include "GenericNodes.hpp"
#include "StreamNode.hpp"
#include "app_config.hpp"
#include "cg_enums.h"

using namespace arm_cmsis_stream;

// This template can only be instantiated with OUT = float,
// but we keep it generic for demonstration purposes.
template <typename IN, int inputSamples>
class DebugSink : public GenericSink<IN, inputSamples> {
public:
  enum selector {selMessage=0};
  static std::array<uint16_t,1> selectors;
  DebugSink(FIFOBase<IN> &dst,EventQueue *queue)
      : GenericSink<IN, inputSamples>(dst),ev0(queue),nb(0),total_nb(0) {

        };

  ~DebugSink() {

  };

  int run() final {
    IN *input = this->getReadBuffer();

    // Send an async event "message" on output 0
    ev0.sendAsync(kNormalPriority,selectors[selMessage],input[0]);
    nb++;
    total_nb++;
    // Every 10 blocks, send a message to the application handler. This is just an example of how to use events for application communication; 
    // real applications may want to send more meaningful data or trigger events based on specific conditions.
    if (nb == 10)
    {
      nb = 0;
      // We use a node id of zero. In this example, the application handler doesn't differentiate between nodes, but in a real application 
      // you might want to include the node id or other context in the event data.
      ev0.sendAsyncToApp<int32_t>(0,kNormalPriority,kValue,1);
    }

    // Test error handling.
    // Errors are sent to application handler
    if (total_nb == 20)
    {
       return(CG_OS_ERROR);
    }

    return (CG_SUCCESS);
  };

  void subscribe(int outputPort,StreamNode &dst,int dstPort) final
  {
        ev0.subscribe(dst,dstPort);
  }

protected:
  EventOutput ev0;
  uint32_t nb;
  uint32_t total_nb;
};