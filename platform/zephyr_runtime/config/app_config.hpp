#pragma once

/*
 * Application configuration for a CMSIS-Stream scheduler on Zephyr.
 *
 * Generated schedulers include this copied configuration header before
 * stream_platform_config.hpp. Use it for scheduler-specific application hooks.
 * Shared runtime sizing, thread priorities, stack sizes, pool sections, and
 * logging levels are Zephyr Kconfig symbols; see the example prj.conf.
 */

#include <zephyr/kernel.h>
extern "C" {
#include "hello_params.h"
}

#include "cg_enums.h"
#include "stream_rtos_events.h"

extern struct k_event cg_streamEvent;

// FIFO buffers are placed in a dedicated section so the linker can apply the
// memory layout selected for the generated graph.
// When memory optimization is used by the Python generator, FIFO memory can be
// shared and must remain aligned to avoid unaligned accesses.
#define CG_BEFORE_BUFFER __aligned(16) __attribute__((section(".bss.stream_fifo")))

#define CMSISSTREAM_LOG_ERR(...) LOG_ERR(__VA_ARGS__)
#define CMSISSTREAM_LOG_DBG(...) LOG_DBG(__VA_ARGS__)

#define CG_BEFORE_NODE_EXECUTION(id)                                          \
    {                                                                         \
        uint32_t res = k_event_wait(&cg_streamEvent,                          \
                                    STREAM_PAUSE_EVENT | STREAM_DONE_EVENT,   \
                                    false, K_NO_WAIT);                        \
        if ((res & STREAM_DONE_EVENT) != 0U)                                  \
        {                                                                     \
            k_event_clear(&cg_streamEvent, STREAM_DONE_EVENT);                \
            cgStaticError = CG_STOP_SCHEDULER;                                \
            goto errorHandling;                                               \
        }                                                                     \
        if ((res & STREAM_PAUSE_EVENT) != 0U)                                 \
        {                                                                     \
            k_event_clear(&cg_streamEvent, STREAM_PAUSE_EVENT);               \
            cgStaticError = CG_PAUSED_SCHEDULER;                              \
            goto errorHandling;                                               \
        }                                                                     \
    }
