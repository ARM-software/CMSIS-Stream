#pragma once

/*
 * Application configuration for a CMSIS-Stream scheduler on the POSIX runtime.
 *
 * Generated schedulers include this copied configuration header before
 * stream_platform_config.hpp. Use it for scheduler-specific application hooks.
 * Put shared POSIX runtime overrides in stream_runtime_config.hpp.
 */

#include "cg_enums.h"
#include "stream_rtos_events.h"
#include "stream_runtime_init.hpp"

extern "C" {
#include "hello_params.h"
}

// FIFO buffers are aligned for generated schedules that use memory sharing.
#define CG_BEFORE_BUFFER alignas(16)

#define CG_BEFORE_NODE_EXECUTION(id)                                                \
    {                                                                               \
        uint32_t res = cg_streamEvent.wait(STREAM_PAUSE_EVENT | STREAM_DONE_EVENT,  \
                                           true, 0);                                \
        if ((res & STREAM_DONE_EVENT) != 0U)                                        \
        {                                                                           \
            cgStaticError = CG_STOP_SCHEDULER;                                      \
            goto errorHandling;                                                     \
        }                                                                           \
        if ((res & STREAM_PAUSE_EVENT) != 0U)                                       \
        {                                                                           \
            cgStaticError = CG_PAUSED_SCHEDULER;                                    \
            goto errorHandling;                                                     \
        }                                                                           \
    }
