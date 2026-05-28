#pragma once

/**
 * @file stream_init.hpp
 * @brief Public initialization interface for CMSIS-Stream POSIX applications.
 */

#include "EventQueue.hpp"
#include "StreamNode.hpp"
#include "stream_runtime_init.hpp"

#define NB_APPS 1

extern stream_execution_context_t contexts[NB_APPS];
extern int currentNetwork;

void stream_configure_and_start();

/**
 * @brief Stop stream execution and release all stream resources.
 *
 * @param callerIsRuntimeThread Use false when called from a normal control
 * thread after stream execution has ended. Use true when called from an
 * application handler or graph node running on a runtime thread. Peer runtime
 * threads are still stopped before graph resources are freed.
 */
void stream_free_all(bool callerIsRuntimeThread = false);
