/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        IdentifiedNode.hpp
 * Description:  Helper template that exposes identified C++ nodes through C
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2026 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <type_traits>

extern "C" {
#include "cstream_node.h"
}

#include "StreamNode.hpp"

template <typename T>
CStreamNode createStreamNode(T &obj)
{
    const StreamNodeInterface *stream_intf_current = nullptr;
    const ContextSwitchInterface *context_switch_intf_current = nullptr;

    if constexpr (std::is_base_of<arm_cmsis_stream::StreamNode, T>::value) {
        static const StreamNodeInterface stream_intf = {
            [](const void *self) -> int {
                return static_cast<const T *>(self)->nodeID();
            },
            [](const void *self) -> int {
                return static_cast<const T *>(self)->needsAsynchronousInit();
            },
            [](void *self, int outputPort, CStreamNode *dst, int dstPort) {
                static_cast<T *>(self)->subscribe(outputPort,
                    *static_cast<arm_cmsis_stream::StreamNode *>(dst->obj),
                    dstPort);
            },
            [](void *self) -> cg_status {
                return static_cast<T *>(self)->init();
            },
        };
        stream_intf_current = &stream_intf;
    }

    if constexpr (std::is_base_of<ContextSwitch, T>::value) {
        static const ContextSwitchInterface context_switch_intf = {
            [](void *self) -> int {
                return static_cast<T *>(self)->pause();
            },
            [](void *self) -> int {
                return static_cast<T *>(self)->resume();
            },
        };
        context_switch_intf_current = &context_switch_intf;
    }

    return CStreamNode{&obj, stream_intf_current, context_switch_intf_current};
}
