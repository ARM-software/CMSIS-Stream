/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cstream_node.h
 * Description:  Example C interface to access API of a C++ object
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
 *
 * Copyright (C) 2026 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef C_STREAM_NODE_H
#define C_STREAM_NODE_H

#include "cg_enums.h"

#ifdef   __cplusplus
extern "C"
{
#endif

struct CStreamNode;

/**
 * @brief C interface to access standard StreamNode API shared by all nodes
 * 
 */
struct StreamNodeInterface
{
    int (*nodeID)(const void *self);
    int (*needsAsynchronousInit)(const void *self);
    void (*subscribe)(void *self,int outputPort,CStreamNode *dst,int dstPort);
    cg_status (*init)(void *self);
};


/**
 * @brief C interface to access ContextSwitch API
 * This interface is implemented by nodes that need to do something during
 * context switch. Generally this is used for nodes interacting with
 * hardware peripherals and for nodes having a memory.
 * 
 */
struct ContextSwitchInterface
{
    int (*pause)(void *self);
    int (*resume)(void *self);
};

/**
 * @brief C structure to hold a pointer to a C++ object and its interfaces
 * If the object does not implement a given interface, the corresponding pointer is set to NULL
 * This interface is generated only for nodes marked as identified in the Python description
 * of the graph.
 * If you don't need to interact with a node from outside of the graph, you don't need to
 * generate an interface for this node.
 * Node that to implement context switching, an interaction with some nodes from the outside
 * is required so those nodes must be marked as identified in the Python.
 * 
 * If your application requires additional APIs to interact with some nodes, you can add additional interfaces and methods to this structure.
 */

struct CStreamNode
{
    void *obj;
    const struct StreamNodeInterface *stream_intf;
    const struct ContextSwitchInterface *context_switch_intf;
};

#ifdef   __cplusplus
}
#endif

#endif
