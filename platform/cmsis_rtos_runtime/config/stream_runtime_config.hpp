#pragma once

/*
 * Shared CMSIS-Stream runtime configuration.
 *
 * Define runtime and stream-core overrides here when the target application
 * needs values different from the library defaults. This copied configuration
 * header is included by stream_platform_config.hpp before the platform defaults
 * are defined, so any CMSISSTREAM_* macro placed here overrides the default
 * used by CMSIS-Stream runtime sources and generated schedulers.
 *
 * Keep application-only scheduler hooks in app_config.hpp.
 */


// Uncomment and adapt these macros to route CMSIS-Stream logs through the
// application logging backend.
//#define CMSISSTREAM_LOG_ERR(fmt, ...) fprintf(stderr, "[ERR] " fmt, ##__VA_ARGS__)
//#define CMSISSTREAM_LOG_DBG(fmt, ...) fprintf(stderr, "[DBG] " fmt, ##__VA_ARGS__)
