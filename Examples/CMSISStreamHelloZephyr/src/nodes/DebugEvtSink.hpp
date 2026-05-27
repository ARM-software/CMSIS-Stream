#pragma once

#include <cstdio>

using namespace arm_cmsis_stream;

class DebugEvtSink: public StreamNode
{
public:
    enum selector {selMessage=0};
    static std::array<uint16_t,1> selectors;

    DebugEvtSink():StreamNode(),eventCount(0){};

    void messageReceived(float v)
    {
        printf("[%02d] Message received: %f\n", eventCount, (double)v);
        eventCount++;
    }

    cg_status processEvent(int dstPort,Event &&evt) final
    {
        // "message" event received
        if (evt.event_id == selectors[selMessage])
        {
            if (evt.wellFormed<float>())
            {
                evt.apply<float>(&DebugEvtSink::messageReceived, *this);
            }
            
        }
        return CG_SUCCESS;
        
        
    }

protected:
   int eventCount;

};
