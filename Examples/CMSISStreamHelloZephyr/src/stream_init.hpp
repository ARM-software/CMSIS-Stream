#pragma once

#include "app_config.hpp"

/**
 * @file stream_init.hpp
 * @brief Public initialization interface for CMSIS-Stream Zephyr applications.
 *
 * This copied configuration header exposes the top-level entry points and
 * stream execution contexts used by the application. Adapt NB_APPS and the
 * matching implementation in stream_init.cpp when the project contains more
 * than one generated graph.
 */

#include "EventQueue.hpp"
#include "StreamNode.hpp"

#include "stream_runtime_init.hpp"

/**
 * @brief Number of stream applications configured for this project.
 */
#define NB_APPS 1

/**
 * @brief Stream execution contexts, one entry per available application.
 */
extern stream_execution_context_t contexts[NB_APPS];

/**
 * @brief Index of the stream application currently selected for execution.
 */
extern int currentNetwork;

/**
 * @brief Configure hardware, initialize stream graphs, and start the selected graph.
 */
void stream_configure_and_start();

/**
 * @brief Stop stream execution and release all stream resources.
 *
 * The optional argument controls whether the function waits for runtime threads
 * to finish before releasing resources.
 */
void stream_free_all(bool mustWait = true);
