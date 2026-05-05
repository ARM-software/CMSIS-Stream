#pragma once

/*

This file can be used to override default configuration values for the stream runtime and stream core templates.
It is included by all source files in the stream runtime, so any definition here will be visible throughout the runtime implementation.

The application configuration header used by generated schedulers must include this file.

*/


//#define CMSISSTREAM_LOG_ERR(fmt, ...) fprintf(stderr, "[ERR] " fmt, ##__VA_ARGS__)
//#define CMSISSTREAM_LOG_DBG(fmt, ...) fprintf(stderr, "[DBG] " fmt, ##__VA_ARGS__)
