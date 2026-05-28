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
void stream_free_all(bool mustWait = true);
