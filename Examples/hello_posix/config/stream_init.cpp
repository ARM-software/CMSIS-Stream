/**
 * @file stream_init.cpp
 * @brief Configure, start, and release CMSIS-Stream POSIX applications.
 *
 * This copied configuration file connects generated schedulers to the shared
 * POSIX runtime. Adapt it for the generated graph headers, application
 * parameter blocks, event queues, and execution contexts used by the target
 * application.
 */

#include "app_config.hpp"

extern "C" {
#include "hello_params.h"
}

#include "scheduler_hello.h"

#include "EventQueue.hpp"
#include "StreamNode.hpp"
#include "cstream_node.h"
#include "stream_init.hpp"
#include "stream_runtime_init.hpp"

#include <cstdint>
#include <cstdlib>

using namespace arm_cmsis_stream;

int currentNetwork = 0;
stream_execution_context_t contexts[NB_APPS];
EventQueue *queue_app[NB_APPS];

static void pause_scheduler_app(const stream_execution_context_t *context)
{
    for (int32_t nodeid = 0; nodeid < static_cast<int32_t>(context->nb_identified_nodes); nodeid++) {
        CStreamNode *cnode = static_cast<CStreamNode *>(context->get_node_by_id(nodeid));
        if ((cnode != nullptr) &&
            (cnode->obj != nullptr) &&
            (cnode->context_switch_intf != nullptr)) {
            cnode->context_switch_intf->pause(cnode->obj);
        }
    }
}

static void resume_scheduler_app(const stream_execution_context_t *context)
{
    for (int32_t nodeid = 0; nodeid < static_cast<int32_t>(context->nb_identified_nodes); nodeid++) {
        CStreamNode *cnode = static_cast<CStreamNode *>(context->get_node_by_id(nodeid));
        if ((cnode != nullptr) &&
            (cnode->obj != nullptr) &&
            (cnode->context_switch_intf != nullptr)) {
            cnode->context_switch_intf->resume(cnode->obj);
        }
    }
}

static void *get_hello_node(int32_t nodeID)
{
    return static_cast<void *>(get_scheduler_hello_node(nodeID));
}

static void handle_error(int32_t origin, int32_t error_code, int32_t info)
{
    (void)info;
    CMSISSTREAM_LOG_ERR("Error from origin %d with code %d\n", origin, error_code);
    /*
     * This handler can be called from a runtime thread or from a graph node.
     * stream_free_all(true) may delete nodes, FIFOs, queues, and scheduler
     * resources that are still present on the current call stack. It is safe
     * here because the example exits immediately after freeing resources.
     *
     * If an application wants to recover and restart a graph, do not return
     * from this handler after freeing resources. Instead, signal a supervisor
     * thread and let that thread stop/free/recreate/start the graph.
     */
    stream_free_all(true);
    std::exit(1);
}

static bool application_handler(int src_node_id, void *data, Event &&evt)
{
    int network_id = static_cast<int>(reinterpret_cast<intptr_t>(data));
    if (evt.event_id == kValue) {
        int32_t msg = evt.get<int32_t>();
        CMSISSTREAM_LOG_DBG("Application handler received event from node %d in network %d with value: %d\n",
                            src_node_id, network_id, msg);
    } else if (evt.event_id == kError) {
        if (evt.wellFormed<int32_t, int32_t, int32_t>()) {
            evt.apply<int32_t, int32_t, int32_t>(&handle_error);
        }
    } else if (evt.event_id == kStopGraph) {
        CMSISSTREAM_LOG_DBG("Application handler received stop graph event for network\n");
    } else {
        CMSISSTREAM_LOG_DBG("Application handler received unknown event ID %d from node %d in network %d\n",
                            evt.event_id, src_node_id, network_id);
    }
    return true;
}

void stream_configure_and_start()
{
    int err;

    helloParams.src.val = 1.5f;
    helloParams.src.hw_.delay_ms = 100;

    err = stream_init_memory();
    if (err != 0) {
        CMSISSTREAM_LOG_ERR("Error initializing stream\n");
        goto error;
    }

    for (int network = 0; network < NB_APPS; network++) {
        queue_app[network] = stream_new_event_queue();

        if (queue_app[network] == nullptr) {
            CMSISSTREAM_LOG_ERR("Can't create CMSIS Stream Event Queue for network %d\n", network);
            goto error;
        }
        queue_app[network]->setHandler(reinterpret_cast<void *>(static_cast<intptr_t>(network)),
                                       application_handler);
    }

    err = init_scheduler_hello(queue_app[0], &helloParams);
    if (err != CG_SUCCESS) {
        CMSISSTREAM_LOG_ERR("Error: Failure during scheduler initialization for hello graph.\n");
        goto error;
    }

    contexts[0] = stream_execution_context_t{
        scheduler_hello,
        reset_fifos_scheduler_hello,
        pause_scheduler_app,
        resume_scheduler_app,
        get_hello_node,
        queue_app[0],
        STREAM_HELLO_NB_IDENTIFIED_NODES,
        STREAM_HELLO_SCHED_LEN};

    resume_scheduler_app(&contexts[currentNetwork]);
    if (!stream_start_threads(&contexts[currentNetwork])) {
        CMSISSTREAM_LOG_ERR("Error starting stream runtime threads\n");
        goto error;
    }

    return;

error:
    CMSISSTREAM_LOG_ERR("Fatal error in main, stopping execution\n");
}

void stream_free_all(bool callerIsRuntimeThread)
{
    stream_stop_threads(callerIsRuntimeThread);

    free_scheduler_hello();

    for (int network = 0; network < NB_APPS; network++) {
        delete queue_app[network];
        queue_app[network] = nullptr;
    }

    stream_free_memory();
}
