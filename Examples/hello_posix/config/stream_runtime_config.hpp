#pragma once

/*
 * Shared CMSIS-Stream POSIX runtime configuration.
 *
 * Define runtime and stream-core overrides here when the target application
 * needs values different from the library defaults. This copied configuration
 * header is included by stream_platform_config.hpp before platform defaults
 * are defined.
 */

#include <cstdio>

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Event Queue Configuration

// <o CMSISSTREAM_EVENT_QUEUE_LENGTH>Event queue length <1..1024>
// <i>Maximum number of events that can wait in one CMSIS-Stream event queue.
// <d> 20
#define CMSISSTREAM_EVENT_QUEUE_LENGTH 20

// </h>

// <h>Runtime Thread Configuration

// <o CMSISSTREAM_STREAM_THREAD_PRIORITY>Stream thread priority
// <ThreadPriority::Low=> Low
// <ThreadPriority::Normal=> Normal
// <ThreadPriority::High=> High
// <ThreadPriority::RealTime=> Realtime
// <d> ThreadPriority::RealTime
#define CMSISSTREAM_STREAM_THREAD_PRIORITY ThreadPriority::RealTime

// <o CMSISSTREAM_EVT_HIGH_PRIORITY>Event thread high priority
// <ThreadPriority::Low=> Low
// <ThreadPriority::Normal=> Normal
// <ThreadPriority::High=> High
// <ThreadPriority::RealTime=> Realtime
// <d> ThreadPriority::High
#define CMSISSTREAM_EVT_HIGH_PRIORITY ThreadPriority::High

// <o CMSISSTREAM_EVT_NORMAL_PRIORITY>Event thread normal priority
// <ThreadPriority::Low=> Low
// <ThreadPriority::Normal=> Normal
// <ThreadPriority::High=> High
// <ThreadPriority::RealTime=> Realtime
// <d> ThreadPriority::Normal
#define CMSISSTREAM_EVT_NORMAL_PRIORITY ThreadPriority::Normal

// <o CMSISSTREAM_EVT_LOW_PRIORITY>Event thread low priority
// <ThreadPriority::Low=> Low
// <ThreadPriority::Normal=> Normal
// <ThreadPriority::High=> High
// <ThreadPriority::RealTime=> Realtime
// <d> ThreadPriority::Low
#define CMSISSTREAM_EVT_LOW_PRIORITY ThreadPriority::Low

// </h>

// <h>Event Data Configuration

// <o CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS>Maximum number of event arguments <1..64>
// <d> 8
#define CMSISSTREAM_MAX_NUMBER_EVENT_ARGUMENTS 8

// <o CMSISSTREAM_TENSOR_MAX_DIMENSIONS>Maximum number of tensor dimensions <1..8>
// <d> 3
#define CMSISSTREAM_TENSOR_MAX_DIMENSIONS 3

// </h>

// <<< end of configuration section >>>

#define CMSISSTREAM_LOG_ERR(fmt, ...) std::fprintf(stderr, "[ERR] " fmt, ##__VA_ARGS__)
#define CMSISSTREAM_LOG_DBG(fmt, ...) std::fprintf(stderr, "[DBG] " fmt, ##__VA_ARGS__)
