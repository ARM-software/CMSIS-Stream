#pragma once

/**
 * @file stream_init.hpp
 * @brief Public initialization interface for the CMSIS-Stream demo graphs.
 *
 * This module owns the top-level stream execution contexts and exposes the
 * functions used by the application entry point to configure, start, and free
 * the available stream applications.
 */

#include "EventQueue.hpp"
#include "StreamNode.hpp"

#include "stream_runtime_init.hpp"

/**
 * @brief Number of stream applications available in this demo.
 */
#define NB_APPS 1

/**
 * @brief Stream execution contexts, one entry per available application.
 *
 * Each context binds a generated scheduler, FIFO reset function, node access
 * wrapper, event queue, and metadata needed by the runtime context switch code.
 */
extern stream_execution_context_t contexts[NB_APPS];

/**
 * @brief Index of the stream application currently selected for execution.
 *
 * The index selects an entry in @ref contexts and the corresponding event queue.
 */
extern int currentNetwork;

/**
 * @brief Configure hardware, initialize stream graphs, and start the selected graph.
 *
 * The function initializes the required hardware drivers, prepares graph
 * parameters, allocates runtime memory and event queues, initializes generated
 * schedulers, builds stream execution contexts, then starts the active graph.
 */
void stream_configure_and_start();

/**
 * @brief Stop stream execution and release all stream resources.
 *
 * The function waits for runtime threads to finish, frees generated scheduler
 * resources, destroys event queues, and releases CMSIS-Stream runtime memory.
 */
void stream_free_all();
