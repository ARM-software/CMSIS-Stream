/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        IdentifiedNode.h
 * Description:  Example C++ template to create a C interface to a C++ object
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
 *
 * Copyright (C) 2021-2025 ARM Limited or its affiliates. All rights reserved.
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
#pragma once 

extern "C" {
   #include "cstream_node.h"
}

#include "StreamNode.hpp"

template <typename T>
CStreamNode createStreamNode(T &obj)
{
    if constexpr (std::is_base_of<arm_cmsis_stream::StreamNode, T>::value)
    {
        static const StreamNodeInterface stream_intf = {
            [](const void *self) -> int {
                return(static_cast<const T *>(self)->nodeID());
            }};
        return CStreamNode{&obj, &stream_intf};
    }
    else
    {
       return  CStreamNode {&obj,nullptr};
    }
};

