#pragma once

/*
 * Shared CMSIS-Stream runtime configuration.
 *
 * Define runtime and stream-core overrides here when the target application
 * needs values different from the library defaults. This header is included by
 * all CMSIS-Stream runtime source files, and app_config.hpp includes it for the
 * generated scheduler, so definitions placed here are visible to both sides.
 *
 * Keep application-only scheduler hooks in app_config.hpp.
 */


// Uncomment and adapt these macros to route CMSIS-Stream logs through the
// application logging backend.
//#define CMSISSTREAM_LOG_ERR(fmt, ...) fprintf(stderr, "[ERR] " fmt, ##__VA_ARGS__)
//#define CMSISSTREAM_LOG_DBG(fmt, ...) fprintf(stderr, "[DBG] " fmt, ##__VA_ARGS__)
