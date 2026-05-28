/**
 * @file stream_init.cpp
 * @brief Configure, start, and release CMSIS-Stream Zephyr applications.
 *
 * This copied configuration file connects generated schedulers to the shared
 * CMSIS-Stream Zephyr runtime. Adapt it for the generated graph headers,
 * application parameter blocks, hardware drivers, event queues, and execution
 * contexts used by the target application.
 */

#include "app_config.hpp"

extern "C" {
#include "hello_params.h"
}

// Generated scheduler header for graph hello. Replace with the generated
// scheduler header used by the application.
#include "scheduler_hello.h"

#include "EventQueue.hpp"
#include "StreamNode.hpp"
#include "cstream_node.h"
#include "stream_runtime_init.hpp"

#include "stream_init.hpp"

#include <zephyr/kernel.h>
#include <cstdint>

using namespace arm_cmsis_stream;

/**
 * @brief Index of the stream application currently selected for execution.
 */
int currentNetwork = 0;

/**
 * @brief Array of stream execution contexts, one per network.
 */
stream_execution_context_t contexts[NB_APPS];

/**
 * @brief Event queues used by each stream application.
 */
EventQueue *queue_app[NB_APPS];

/*
 * Pause all context-switchable nodes in a graph.
 */
static void pause_scheduler_app(const stream_execution_context_t *context)
{
    for (int32_t nodeid = 0; nodeid < (int32_t)context->nb_identified_nodes; nodeid++)
    {
        CStreamNode *cnode = static_cast<CStreamNode *>(context->get_node_by_id(nodeid));
        if ((cnode != nullptr) &&
            (cnode->obj != nullptr) &&
            (cnode->context_switch_intf != nullptr))
        {
            cnode->context_switch_intf->pause(cnode->obj);
        }
    }
}

/*
 * Resume all context-switchable nodes in a graph.
 */
static void resume_scheduler_app(const stream_execution_context_t *context)
{
    for (int32_t nodeid = 0; nodeid < (int32_t)context->nb_identified_nodes; nodeid++)
    {
        CStreamNode *cnode = static_cast<CStreamNode *>(context->get_node_by_id(nodeid));
        if ((cnode != nullptr) &&
            (cnode->obj != nullptr) &&
            (cnode->context_switch_intf != nullptr))
        {
            cnode->context_switch_intf->resume(cnode->obj);
        }
    }
}

/*
 * Wrap generated node accessors behind the runtime's void* API.
 */
static void *get_hello_node(int32_t nodeID)
{
    return static_cast<void *>(get_scheduler_hello_node(nodeID));
}

static void timer_callback(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    CMSISSTREAM_LOG_DBG("Timer callback");
}

K_TIMER_DEFINE(stream_source_timer, timer_callback, nullptr);

static void handle_error(int32_t origin, int32_t error_code, int32_t info)
{
    ARG_UNUSED(info);
    CMSISSTREAM_LOG_ERR("Error from origin %d with code %d", origin, error_code);
    /*
     * This handler can be called from a runtime thread or from a graph node.
     * stream_free_all(true) may delete nodes, FIFOs, queues, and scheduler
     * resources that are still present on the current call stack. It is safe
     * here because the example stops immediately after freeing resources.
     *
     * If an application wants to recover and restart a graph, do not return
     * from this handler after freeing resources. Instead, signal a supervisor
     * thread and let that thread stop/free/recreate/start the graph.
     */
    stream_free_all(true);
    CMSISSTREAM_LOG_DBG("Exiting application");
    printk("\x04");
    k_sleep(K_FOREVER);
}

/**
 * @brief Application-specific event handler.
 */
static bool application_handler(int src_node_id, void *data, Event &&evt)
{
    int network_id = static_cast<int>(reinterpret_cast<intptr_t>(data));
    if (evt.event_id == kValue)
    {
        int32_t msg = evt.get<int32_t>();
        CMSISSTREAM_LOG_DBG("Application handler received event from node %d in network %d with value: %d",
                            src_node_id, network_id, msg);
    }
    else if (evt.event_id == kError)
    {
        if (evt.wellFormed<int32_t, int32_t, int32_t>())
        {
            evt.apply<int32_t, int32_t, int32_t>(&handle_error);
        }
    }
    else if (evt.event_id == kStopGraph)
    {
        CMSISSTREAM_LOG_DBG("Application handler received stop graph event for network");
    }
    else
    {
        CMSISSTREAM_LOG_DBG("Application handler received unknown event ID %d from node %d in network %d",
                            evt.event_id, src_node_id, network_id);
    }
    return true;
}

/**
 * @brief Configure resources and start the currently selected stream graph.
 */
void stream_configure_and_start()
{
    int err;

    /*
     * Step 1: configure hardware sources that may be shared by the stream
     * graphs. Replace this example k_timer with application-specific drivers.
     */

    /*
     * Step 2: fill generated parameter structures with application-specific
     * data before scheduler initialization.
     *
     * For this example, hello_params.h is expected to define a parameter field
     * compatible with a Zephyr timer pointer, such as:
     *     struct k_timer *timer;
     */
    helloParams.src.hw_.timer = &stream_source_timer;
    helloParams.src.val = 1.5f;

    // Step 3: initialize common CMSIS-Stream runtime memory.
    err = stream_init_memory();
    if (err != 0)
    {
        CMSISSTREAM_LOG_ERR("Error initializing stream");
        goto error;
    }

    // Step 4: allocate one event queue per graph.
    for (int network = 0; network < NB_APPS; network++)
    {
        queue_app[network] = stream_new_event_queue();

        if (queue_app[network] == nullptr)
        {
            CMSISSTREAM_LOG_ERR("Can't create CMSIS Stream Event Queue for network %d", network);
            goto error;
        }
        queue_app[network]->setHandler(reinterpret_cast<void *>(static_cast<intptr_t>(network)),
                                       application_handler);
    }

    // Step 5: initialize generated schedulers and graph nodes.
    err = init_scheduler_hello(queue_app[0], &helloParams);
    if (err != CG_SUCCESS)
    {
        CMSISSTREAM_LOG_ERR("Error: Failure during scheduler initialization for hello graph.");
        goto error;
    }

    // Step 6: bind generated scheduler callbacks and metadata to runtime contexts.
    contexts[0] = stream_execution_context_t{
        scheduler_hello,
        reset_fifos_scheduler_hello,
        pause_scheduler_app,
        resume_scheduler_app,
        get_hello_node,
        queue_app[0],
        STREAM_HELLO_NB_IDENTIFIED_NODES,
        STREAM_HELLO_SCHED_LEN};

    // Step 7: resume the selected graph and start the runtime threads.
    resume_scheduler_app(&contexts[currentNetwork]);
    if (!stream_start_threads(&contexts[currentNetwork]))
    {
        CMSISSTREAM_LOG_ERR("Error starting stream runtime threads");
        goto error;
    }

    return;

error:
    CMSISSTREAM_LOG_ERR("Fatal error in main, stopping execution");
}

/**
 * @brief Stop all stream threads and release scheduler/runtime resources.
 */
void stream_free_all(bool callerIsRuntimeThread)
{
    stream_stop_threads(callerIsRuntimeThread);

    free_scheduler_hello();

    for (int network = 0; network < NB_APPS; network++)
    {
        delete queue_app[network];
        queue_app[network] = nullptr;
    }

    stream_free_memory();
}
