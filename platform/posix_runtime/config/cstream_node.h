/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cstream_node.h
 * Description:  C interface used by the runtime to access C++ stream nodes
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2026 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef C_STREAM_NODE_H
#define C_STREAM_NODE_H

#include "cg_enums.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct CStreamNode;

struct StreamNodeInterface
{
    int (*nodeID)(const void *self);
    int (*needsAsynchronousInit)(const void *self);
    void (*subscribe)(void *self, int outputPort, CStreamNode *dst, int dstPort);
    cg_status (*init)(void *self);
};

struct ContextSwitchInterface
{
    int (*pause)(void *self);
    int (*resume)(void *self);
};

struct CStreamNode
{
    void *obj;
    const struct StreamNodeInterface *stream_intf;
    const struct ContextSwitchInterface *context_switch_intf;
};

#ifdef __cplusplus
}
#endif

#endif
