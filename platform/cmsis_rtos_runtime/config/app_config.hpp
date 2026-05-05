#pragma once 

/*

Application configuration file.
This header can be used to configure application specific settings.
The header must include the "stream_runtime_config.hpp" used to configure the CMSIS-Stream runtime parameters.

This application header is only included by the generated scheduler whereas the "stream_runtime_config.hpp" is included by 
both the generated scheduler and the CMSIS-Stream runtime source files.

In case of memory optimization defined in the Python scripts. CG_BEFORE_BUFFER must be defined
to force alignment of FIFO buffers.

CG_BEFORE_NODE_EXECUTION must be defined to interface the generated scheduler with the CMSIS-RTOS runtime, and allow it to react to pause and stop events.
This macro can also be modified if additional code must be executed before each node execution in the generated scheduler.
For instance, for debugging purpose, the node ID could be printed before its execution.

*/

extern "C"
{
#include "RTE_Components.h"
#include CMSIS_device_header

#include "cmsis_os2.h" /* CMSIS-RTOS2 API */
}

extern osEventFlagsId_t cg_streamEvent;

// Because memory optimization is enabled in Python scripts, the alignment is NEEDED
//
// To use a memory overlay for the graph FIFOs, the section must be different for each graph
// Python scripts can be customized so that each generated scheduler includes a different
// configuration file where the macro could have different definitions
#define CG_BEFORE_BUFFER __ALIGNED(16) __attribute__((section(".bss.stream_fifo")))

#define CG_BEFORE_NODE_EXECUTION(id)                                                 \
    {                                                                                \
        uint32_t res =                                                               \
            osEventFlagsWait(cg_streamEvent, STREAM_PAUSE_EVENT | STREAM_DONE_EVENT, \
                             osFlagsWaitAny, 0);                                     \
        if (!(res & 0x80000000))                                                     \
        {                                                                            \
            if ((res & STREAM_DONE_EVENT) != 0)                                      \
            {                                                                        \
                cgStaticError = CG_STOP_SCHEDULER;                                   \
                goto errorHandling;                                                  \
            }                                                                        \
            if ((res & STREAM_PAUSE_EVENT) != 0)                                     \
            {                                                                        \
                cgStaticError = CG_PAUSED_SCHEDULER;                                 \
                goto errorHandling;                                                  \
            }                                                                        \
        }                                                                            \
    }



#include "stream_runtime_config.hpp"