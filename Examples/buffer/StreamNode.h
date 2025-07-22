/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        GenericNodes.h
 * Description:  Main classes and templates for CMSIS-Stream nodes.
 *               All nodes must inherit from StreamNode.
 *               Dataflow nodes must inherit from NodeBase.
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

#ifndef STREAMNODE_H_
#define STREAMNODE_H_

#include <memory>
#include <cstdint>
/* Node ID is -1 when nodes are not identified for the external
world */
#define CG_UNIDENTIFIED_NODE (-1)

namespace arm_cmsis_stream {

    
    enum cg_value_type : uint32_t
    {
        kInt8 = 1,
        kInt16 = 2,
        kInt32 = 3,
        kInt64 = 4,
        kFloat = 5,
        kDouble = 6,
        kUint8 = 7,
        kUint16 = 8,
        kUint32 = 9,
        kUint64 = 10,
        kAny = 11,
        kNone = 12,
        kStr = 13
    };

    struct cg_value
    {

        cg_value() : type(kNone), buf_size(0), buf_type(0)
        {
        }

        ~cg_value()
        {
            if ((type == kAny) || (type == kStr))
            {
               any.reset(); // Clean up allocated memory
            }
        }

        cg_value(const cg_value &other)
        {
            copyFrom(other); // Copy the value from other
        }

        cg_value(cg_value &&other)
        {
            moveFrom(std::move(other)); // Move the value from other
        }

        cg_value &operator=(const cg_value &other)
        {
            if (this != &other)
            {
                copyFrom(other); // Copy the value from other
            }
            return *this; // Return the current object
        }

        cg_value &operator=(cg_value &&other)
        {
            if (this != &other)
            {
                moveFrom(std::move(other)); // Move the value from other
            }
            return *this; // Return the current object
        }

        cg_value(int8_t v) : type(kInt8), buf_size(0), buf_type(0)
        {
            value.i8 = v;
        }
        cg_value(int16_t v) : type(kInt16), buf_size(0), buf_type(0)
        {
            value.i16 = v;
        }
        cg_value(int32_t v) : type(kInt32), buf_size(0), buf_type(0)
        {
            value.i32 = v;
        }
        cg_value(int64_t v) : type(kInt64), buf_size(0), buf_type(0)
        {
            value.i64 = v;
        }
        cg_value(float v) : type(kFloat), buf_size(0), buf_type(0)
        {
            value.f32 = v;
        }
        cg_value(double v) : type(kDouble), buf_size(0), buf_type(0)
        {
            value.f64 = v;
        }
        cg_value(uint8_t v) : type(kUint8), buf_size(0), buf_type(0)
        {
            value.u8 = v;
        }
        cg_value(uint16_t v) : type(kUint16), buf_size(0), buf_type(0)
        {
            value.u16 = v;
        }
        cg_value(uint32_t v) : type(kUint32), buf_size(0), buf_type(0)
        {
            value.u32 = v;
        }
        cg_value(uint64_t v) : type(kUint64), buf_size(0), buf_type(0)
        {
            value.u64 = v;
        }
        cg_value(const char *v, size_t size,uint32_t buftype = 0) : type(kAny), buf_size(size), buf_type(buftype)
        {
            if (v != nullptr)
            {
                char* buffer = new char[size];
                std::memcpy(buffer, v, size);  // Copy contents
                any = std::shared_ptr<void>(static_cast<void*>(buffer),
                       [](void* p) { delete[] static_cast<char*>(p); });
            }
            else
            {
                buf_size = 0;
                any = nullptr;
            }
        }

        cg_value(std::shared_ptr<void> &v,size_t size,uint32_t buftype = 0) : type(kAny), buf_size(size), buf_type(buftype)
        {
            if (v != nullptr)
            {
                any = v;
            }
            else
            {
                buf_size = 0;
                any = nullptr;
            }
        }

        cg_value(const char *s) : type(kStr),buf_type(0)
        {
            buf_size = strlen(s) + 1;

            char* buffer = new char[buf_size];
            std::memcpy(buffer, s, buf_size);  // Copy contents
            any = std::shared_ptr<void>(static_cast<void*>(buffer),
                       [](void* p) { delete[] static_cast<char*>(p); });

           
        }
    
        enum cg_value_type type;
        uint32_t buf_size; // Size in bytes of the value
        uint32_t buf_type; 
        std::shared_ptr<void> any;
        union
        {
            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;
            float f32;
            double f64;
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint64_t u64;
        } value;


    protected:
        void moveFrom(cg_value &&other)
        {
            type = other.type;
            buf_size = other.buf_size;
            buf_type = other.buf_type;
            switch (type)
            {
            case kInt8:
                value.i8 = other.value.i8;
                break;
            case kInt16:
                value.i16 = other.value.i16;
                break;
            case kInt32:
                value.i32 = other.value.i32;
                break;
            case kInt64:
                value.i64 = other.value.i64;
                break;
            case kFloat:
                value.f32 = other.value.f32;
                break;
            case kDouble:
                value.f64 = other.value.f64;
                break;
            case kUint8:
                value.u8 = other.value.u8;
                break;
            case kUint16:
                value.u16 = other.value.u16;
                break;
            case kUint32:
                value.u32 = other.value.u32;
                break;
            case kUint64:
                value.u64 = other.value.u64;
                break;
            default:
                // Handle any type
                if ((type == kAny) || (type == kStr))
                {
                    any = std::move(other.any); // Move the pointer
                    other.buf_size = 0;          // Reset size of moved object
                }
            }
            other.type = kNone; // Reset type of moved object
        }

        void copyFrom(const cg_value &other)
        {
            type = other.type;
            buf_size = other.buf_size;
            buf_type = other.buf_type;
            switch (type)
            {
            case kInt8:
                value.i8 = other.value.i8;
                break;
            case kInt16:
                value.i16 = other.value.i16;
                break;
            case kInt32:
                value.i32 = other.value.i32;
                break;
            case kInt64:
                value.i64 = other.value.i64;
                break;
            case kFloat:
                value.f32 = other.value.f32;
                break;
            case kDouble:
                value.f64 = other.value.f64;
                break;
            case kUint8:
                value.u8 = other.value.u8;
                break;
            case kUint16:
                value.u16 = other.value.u16;
                break;
            case kUint32:
                value.u32 = other.value.u32;
                break;
            case kUint64:
                value.u64 = other.value.u64;
                break;
            default:
                // Handle any type
                if ((type == kAny) || (type == kStr))
                {
                    any = other.any;
                }
            }
        }
    };

    class Event
    {

    public:
        Event(uint32_t id, 
              const cg_value &value,
              enum cg_event_priority evtPriority = kNormalPriority)
            : event_id(id), data(value),priority(0) 
            {
                switch(evtPriority)
                {
                    case kLowPriority:
                        priority = 1;
                        break;
                    case kNormalPriority:
                        priority = 2;
                        break;
                    case kHighPriority:
                        priority = 3;
                        break;
                    default:
                        priority = 2; // Default to normal priority
                }
            }
        
        Event(const Event &other)
        {
            copyFrom(other);
        }

        Event(Event &&other)
        {
            moveFrom(std::move(other));
        }
        Event &operator=(const Event &other)
        {
            if (this != &other)
            {
                copyFrom(other);
            }
            return *this;
        }

        Event &operator=(Event &&other)
        {
            if (this != &other)
            {
                moveFrom(std::move(other));
            }
            return *this;
        }

        uint32_t event_id;
        uint32_t priority;
        cg_value data;

    protected:
        void moveFrom(Event &&other)
        {
            event_id = other.event_id;
            data = std::move(other.data);
            priority = other.priority;
            other.event_id = kNoEvent;
        }

        void copyFrom(const Event &other)
        {
            event_id = other.event_id;
            data = other.data;
            priority = other.priority;
        }
    };

class StreamNode
{
public:
    virtual ~StreamNode() {};

    StreamNode(){};

    virtual void processEvent(int dstPort, const Event &evt) {};
    virtual void processEvent(int dstPort, Event &&evt) {};

    /* 
    Nodes are fixed and not made to be copied or moved.
    */
    StreamNode(const StreamNode&) = delete;
    StreamNode(StreamNode&&) = delete;
    StreamNode& operator=(const StreamNode&) = delete;
    StreamNode& operator=(StreamNode&&) = delete;

    void setID(int id)   {mNodeID = id;};
    int nodeID() const   {return(mNodeID);};

private:
    int mNodeID = CG_UNIDENTIFIED_NODE;
};
}; // end namespace

#endif
