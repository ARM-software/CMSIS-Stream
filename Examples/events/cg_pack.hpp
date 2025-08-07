/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        cg_pack.hpp
 * Description:  Serialization / unserialization of events (for RPC)
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * --------------------------------------------------------------------
 *
 * Copyright (C) 2023-2025 ARM Limited or its affiliates. All rights reserved.
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

#include "cg_enums.h"
#include "StreamNode.hpp"
#include <vector>
#include <variant>
#include <string>
#include <cstddef>

namespace arm_cmsis_stream
{

    // Used to receive events from network
    enum cg_value_type
    {
        kNone = 0, // No value
        kInt8 = 1,
        kInt16 = 2,
        kInt32 = 3,
        kInt64 = 4,
        kFloat = 5,
        kDouble = 6,
        kUInt8 = 7,
        kUInt16 = 8,
        kUInt32 = 9,
        kUInt64 = 10,
        kAny = 11, // Generic raw buffer
        kStr = 12,
        kTensor = 13, // Cover const and non const tensors
        kCombined = 14
    };

    // Used to receive events from network
    enum cg_buffer_type
    {
        kCopyBuffer = 0,  // Buffer to copy
        kSharedBuffer = 1 // File descriptor to send
    };

    class Pack
    {
    public:
        Pack() = default;

        Pack(const std::vector<uint8_t> &data) : serialized_object(data) {}

        Pack(std::vector<uint8_t> &&data) : serialized_object(std::move(data)) {}

        const std::vector<uint8_t> &vector() const { return serialized_object; }

        void reset()
        {
            serialized_object.clear();
        }

        size_t size() const
        {
            return serialized_object.size();
        }

        // If we send an event outside on the network
        // the shared buffers are not transmitted so we should not
        // acquire them
        void pack(uint32_t nodeid, Event evt,bool network=false)
        {
            write_value((uint32_t)nodeid);
            write_value((uint32_t)evt.event_id);
            write_value((uint8_t)evt.priority);
            if (std::holds_alternative<cg_value>(evt.data))
            {
                write_value(uint8_t(0));
                pack(std::get<cg_value>(evt.data),network);
            }
            else if (std::holds_alternative<std::shared_ptr<ListValue>>(evt.data))
            {
                write_value(uint8_t(1));
                const ListValue &cv = *std::get<std::shared_ptr<ListValue>>(evt.data);
                write_value(cv.nb_values);
                for (uint32_t i = 0; i < cv.nb_values; ++i)
                {
                    pack(cv.values[i],network);
                }
            }
        };

    protected:
        void write_value(uint8_t res)
        {
            serialized_object.emplace_back(uint8_t(res & 0xFF));
        }

        void write_value(uint16_t res)
        {
            serialized_object.emplace_back(uint8_t(res & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 8) & 0xFF));
        }

        void write_value(uint32_t res)
        {
            serialized_object.emplace_back(uint8_t(res & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 8) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 16) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 24) & 0xFF));
        }

        void write_value(int32_t val)
        {
            uint32_t res;
            memcpy(&res, &val, sizeof(int32_t));
            write_value(res);
        }

        void write_value(uint64_t res)
        {
            serialized_object.emplace_back(uint8_t(res & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 8) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 16) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 24) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 32) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 40) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 48) & 0xFF));
            serialized_object.emplace_back(uint8_t((res >> 56) & 0xFF));
        }

        template <typename T>
        void pack_array(const T *d, std::size_t nbelems)
        {
            write_value((uint32_t)nbelems);
            serialized_object.insert(serialized_object.end(), (uint8_t *)d, (uint8_t *)(d + nbelems));
        }

        template <typename T>
        void pack_shared(const SharedBuffer<T> &buf,
                         std::size_t nbelems,
                         bool network)
        {
            // Record file descriptor to send later
            if (buf.validFd())
            {
                write_value((uint32_t)nbelems);
                write_value((int32_t)(buf.getDescriptor()->global_id()));

                if ((MemServer::mem_server != nullptr) && (!network))
                {
                    MemServer::mem_server->acquire(buf.getDescriptor()->global_id());
                }
                return;
            }
            // If file descriptor is not valid, we write 0 elements for this buffer
            write_value((uint32_t)0);
            write_value((int32_t)-1);
        }

        template <typename T, int N>
        void pack_array(const std::array<T, N> &t)
        {
            write_value((uint32_t)N);
            serialized_object.insert(serialized_object.end(), (uint8_t *)t.data(), (uint8_t *)(t.data() + N));
        }

        template <typename T>
        void pack_tensor(const cg_any_tensor &anyt,
                         uint8_t dt,
                         bool network)
        {
            TensorPtr<T> t = std::get<TensorPtr<T>>(anyt);
            write_value(uint8_t(kTensor));
            t.lock_shared([this, dt,network](CG_MUTEX_ERROR_TYPE error, const Tensor<T> &v)
                          {
                write_value(v.nb_dims);
                pack_array<uint32_t,CG_TENSOR_NB_DIMS>(v.dims);
                write_value(uint8_t(dt));
                if (std::holds_alternative<SharedBuffer<T>>(v.data))
                {
                    const SharedBuffer<T> &buf = std::get<SharedBuffer<T>>(v.data);
                    write_value(uint8_t(kSharedBuffer));
                    pack_shared(buf, v.size(),network);
                }
                else if (std::holds_alternative<UniquePtr<T>>(v.data))
                {
                    const UniquePtr<T> &buf = std::get<UniquePtr<T>>(v.data);
                    write_value(uint8_t(kCopyBuffer));
                    pack_array(buf.get(), v.size());
                } });
        }

        template <typename T>
        void pack_const_tensor(const cg_any_const_tensor &anyt,
                               uint8_t dt,
                               bool network)
        {
            TensorPtr<T> t = std::get<TensorPtr<T>>(anyt);
            write_value(uint8_t(kTensor));
            t.lock_shared([this, dt,network](CG_MUTEX_ERROR_TYPE error, const Tensor<T> &v)
                          {
                write_value(v.nb_dims);
                pack_array<uint32_t,CG_TENSOR_NB_DIMS>(v.dims);
                write_value(uint8_t(dt));
                if (std::holds_alternative<SharedBuffer<T>>(v.data))
                {
                    const SharedBuffer<T> &buf = std::get<SharedBuffer<T>>(v.data);
                    write_value(uint8_t(kSharedBuffer));
                    pack_shared(buf, v.size(),network);
                }
                else if (std::holds_alternative<UniquePtr<T>>(v.data))
                {
                    const UniquePtr<T> &buf = std::get<UniquePtr<T>>(v.data);
                    write_value(uint8_t(kCopyBuffer));
                    pack_array(buf.get(), v.size());
                } });
        }

        void pack(const cg_value &val,bool network)
        {
            if (std::holds_alternative<std::monostate>(val.value))
            {
                write_value(uint8_t(kNone));
                return;
            }
            if (std::holds_alternative<uint8_t>(val.value))
            {
                write_value(uint8_t(kUInt8));
                write_value(std::get<uint8_t>(val.value));
            }
            else if (std::holds_alternative<uint16_t>(val.value))
            {
                write_value(uint8_t(kUInt16));
                uint16_t res = std::get<uint16_t>(val.value);
                write_value(res);
            }
            else if (std::holds_alternative<uint32_t>(val.value))
            {
                write_value(uint8_t(kUInt32));
                uint32_t res = std::get<uint32_t>(val.value);
                write_value(res);
            }
            else if (std::holds_alternative<uint64_t>(val.value))
            {
                write_value(uint8_t(kUInt64));
                uint64_t res = std::get<uint64_t>(val.value);
                write_value(res);
            }
            else if (std::holds_alternative<int8_t>(val.value))
            {
                uint8_t res;
                memcpy(&res, &std::get<int8_t>(val.value), sizeof(int8_t));
                write_value(uint8_t(kInt8));
                write_value(res);
            }
            else if (std::holds_alternative<int16_t>(val.value))
            {
                uint16_t res;
                memcpy(&res, &std::get<int16_t>(val.value), sizeof(int16_t));
                write_value(uint8_t(kInt16));
                write_value(res);
            }
            else if (std::holds_alternative<int32_t>(val.value))
            {
                uint32_t res;
                memcpy(&res, &std::get<int32_t>(val.value), sizeof(int32_t));
                write_value(uint8_t(kInt32));
                write_value(res);
            }
            else if (std::holds_alternative<int64_t>(val.value))
            {
                uint64_t res;
                memcpy(&res, &std::get<int64_t>(val.value), sizeof(int64_t));
                write_value(uint8_t(kInt64));
                write_value(res);
            }
            else if (std::holds_alternative<float>(val.value))
            {
                uint32_t res;
                memcpy(&res, &std::get<float>(val.value), sizeof(float));
                write_value(uint8_t(kFloat));
                write_value(res);
            }
            else if (std::holds_alternative<double>(val.value))
            {
                uint64_t res;
                memcpy(&res, &std::get<double>(val.value), sizeof(double));
                write_value(uint8_t(kDouble));
                write_value(res);
            }
            else if (std::holds_alternative<BufferPtr>(val.value))
            {
                write_value(uint8_t(kAny));
                const BufferPtr r = std::get<BufferPtr>(val.value);
                r.lock_shared([this,network](CG_MUTEX_ERROR_TYPE error, const RawBuffer &v)
                              {
                                  if (std::holds_alternative<UniquePtr<std::byte>>(v.data))
                                  {
                                      const UniquePtr<std::byte> &buf = std::get<UniquePtr<std::byte>>(v.data);
                                      write_value(uint8_t(kCopyBuffer));
                                      this->pack_array((const uint8_t *)buf.get(), v.buf_size);
                                  }
                                  else if (std::holds_alternative<SharedBuffer<std::byte>>(v.data))
                                  {
                                      const SharedBuffer<std::byte> &buf = std::get<SharedBuffer<std::byte>>(v.data);
                                      write_value(uint8_t(kSharedBuffer));
                                      this->pack_shared(buf, v.buf_size,network);
                                  } });
            }
            else if (std::holds_alternative<std::string>(val.value))
            {
                write_value(uint8_t(kStr));
                const std::string &s = std::get<std::string>(val.value);
                pack_array((const uint8_t *)(s.c_str()), s.size() + 1);
            }
            else if (std::holds_alternative<cg_any_tensor>(val.value))
            {
                cg_any_tensor anyt = std::get<cg_any_tensor>(val.value);
                if (std::holds_alternative<TensorPtr<int8_t>>(anyt))
                {
                    pack_tensor<int8_t>(anyt, uint8_t(kInt8),network);
                }
                else if (std::holds_alternative<TensorPtr<int16_t>>(anyt))
                {
                    pack_tensor<int16_t>(anyt, uint8_t(kInt16),network);
                }
                else if (std::holds_alternative<TensorPtr<int32_t>>(anyt))
                {
                    pack_tensor<int32_t>(anyt, uint8_t(kInt32),network);
                }
                else if (std::holds_alternative<TensorPtr<int64_t>>(anyt))
                {
                    pack_tensor<int64_t>(anyt, uint8_t(kInt64),network);
                }
                else if (std::holds_alternative<TensorPtr<uint8_t>>(anyt))
                {
                    pack_tensor<uint8_t>(anyt, uint8_t(kUInt8),network);
                }
                else if (std::holds_alternative<TensorPtr<uint16_t>>(anyt))
                {
                    pack_tensor<uint16_t>(anyt, uint8_t(kUInt16),network);
                }
                else if (std::holds_alternative<TensorPtr<uint32_t>>(anyt))
                {
                    pack_tensor<uint32_t>(anyt, uint8_t(kUInt32),network);
                }
                else if (std::holds_alternative<TensorPtr<uint64_t>>(anyt))
                {
                    pack_tensor<uint64_t>(anyt, uint8_t(kUInt64),network);
                }
                else if (std::holds_alternative<TensorPtr<float>>(anyt))
                {
                    pack_tensor<float>(anyt, uint8_t(kFloat),network);
                }
                else if (std::holds_alternative<TensorPtr<double>>(anyt))
                {
                    pack_tensor<double>(anyt, uint8_t(kDouble),network);
                }
            }
            else if (std::holds_alternative<cg_any_const_tensor>(val.value))
            {
                cg_any_const_tensor anyt = std::get<cg_any_const_tensor>(val.value);
                if (std::holds_alternative<TensorPtr<const int8_t>>(anyt))
                {
                    pack_const_tensor<const int8_t>(anyt, uint8_t(kInt8),network);
                }
                else if (std::holds_alternative<TensorPtr<const int16_t>>(anyt))
                {
                    pack_const_tensor<const int16_t>(anyt, uint8_t(kInt16),network);
                }
                else if (std::holds_alternative<TensorPtr<const int32_t>>(anyt))
                {
                    pack_const_tensor<const int32_t>(anyt, uint8_t(kInt32),network);
                }
                else if (std::holds_alternative<TensorPtr<const int64_t>>(anyt))
                {
                    pack_const_tensor<const int64_t>(anyt, uint8_t(kInt64),network);
                }
                else if (std::holds_alternative<TensorPtr<const uint8_t>>(anyt))
                {
                    pack_const_tensor<const uint8_t>(anyt, uint8_t(kUInt8),network);
                }
                else if (std::holds_alternative<TensorPtr<const uint16_t>>(anyt))
                {
                    pack_const_tensor<const uint16_t>(anyt, uint8_t(kUInt16),network);
                }
                else if (std::holds_alternative<TensorPtr<const uint32_t>>(anyt))
                {
                    pack_const_tensor<const uint32_t>(anyt, uint8_t(kUInt32),network);
                }
                else if (std::holds_alternative<TensorPtr<const uint64_t>>(anyt))
                {
                    pack_const_tensor<const uint64_t>(anyt, uint8_t(kUInt64),network);
                }
                else if (std::holds_alternative<TensorPtr<const float>>(anyt))
                {
                    pack_const_tensor<const float>(anyt, uint8_t(kFloat),network);
                }
                else if (std::holds_alternative<TensorPtr<const double>>(anyt))
                {
                    pack_const_tensor<const double>(anyt, uint8_t(kDouble),network);
                }
            }
        }

    private:
        std::vector<uint8_t> serialized_object;
    };

    class Unpack
    {
    public:
        Unpack() = delete;

        Unpack(const uint8_t *data, std::size_t size) : data_pointer(data) {}

        Event unpack(uint32_t &nodeid)
        {
            nodeid = read_value<uint32_t>();
            uint32_t event_id = read_value<uint32_t>();
            uint8_t priority_nb = read_value<uint8_t>();
            uint8_t dataCase = read_value<uint8_t>();
            enum cg_event_priority priority = kNormalPriority;
            if (priority_nb == 0)
            {
                priority = kLowPriority;
            }
            else if (priority_nb == 1)
            {
                priority = kNormalPriority;
            }
            else if (priority_nb == 2)
            {
                priority = kHighPriority;
            }

            if (dataCase == 0)
            {
                cg_value val = unpack_value();
                return (Event(event_id, priority, std::move(val)));
            }
            else if (dataCase == 1)
            {
                uint32_t nb_values = read_value<uint32_t>();
                std::shared_ptr<ListValue> cv = Event::make_new_list_value();
                cv->nb_values = nb_values;
                for (uint32_t i = 0; i < nb_values; ++i)
                {
                    cv->values[i] = unpack_value();
                }
                return (Event(event_id, cv, priority));
            }
            return (Event((uint32_t)kDo, priority, (cg_value())));
        };

    protected:
        template <typename T>
        T read_value()
        {
            T v;
            memcpy(&v, data_pointer, sizeof(T));
            data_pointer += sizeof(T);
            return v;
        }

        template <typename T, int N>
        void read_array(std::array<T, N> &v)
        {
            // array are always packing their length but we know it from the type
            uint32_t n_unused = read_value<uint32_t>();
            for (int i = 0; i < N; ++i)
            {
                v[i] = read_value<T>();
            }
        }

        template <typename T>
        void read_buffer(T *buf, std::size_t size)
        {
            memcpy(buf, data_pointer, size * sizeof(T));
            data_pointer += size * sizeof(T);
        }

        bool maybe(int k)
        {
            if (*data_pointer == k)
            {
                data_pointer++;
                return true;
            }
            return false;
        }

        template <typename T>
        UniquePtr<T> make_typed_buffer(std::size_t n)
        {
            UniquePtr<T> buf(n);
            read_buffer<T>(buf.get(), n);
            return buf;
        }

        UniquePtr<std::byte> make_raw_buffer(std::size_t n)
        {
            UniquePtr<std::byte> buf(n);
            read_buffer<std::byte>(buf.get(), n);
            return buf;
        }

        std::shared_ptr<char> make_string_buffer(std::size_t n)
        {
            std::shared_ptr<char> buf(new char[n]);
            read_buffer<char>((char *)buf.get(), n);
            return buf;
        }

        template <typename T>
        cg_value unpack_tensor(uint8_t nb_dims,
                               const cg_tensor_dims_t &dims)
        {
            uint8_t buffer_type = read_value<uint8_t>();
            if (buffer_type == kCopyBuffer)
            {
                uint32_t nb = read_value<uint32_t>();
                UniquePtr<T> data = make_typed_buffer<T>(nb);

                TensorPtr<T> buf = TensorPtr<T>::create_with(nb_dims, dims, std::move(data));

                return buf;
            }
            else if (buffer_type == kSharedBuffer)
            {
                uint32_t nb_elems = read_value<uint32_t>();
                int32_t global_id = read_value<int32_t>();
                uint32_t buf_size = nb_elems * sizeof(T);
                if (nb_elems == 0)
                {
                    return cg_value();
                }

                if (MemServer::mem_server == nullptr)
                {
                    return cg_value();
                }
                Descriptor *fd = MemServer::mem_server->get_buffer(global_id);
                if (fd == nullptr)
                {
                    return cg_value();
                }
                // Buffer was acquired by the pack so that the buffer
                // is not destroyed during IPC where no process may have a
                // file descriptor to the buffer.
                // Now that we have a reference with get_buffer we can
                // release the additional reference
                MemServer::mem_server->release(global_id);

                // Create a shared buffer with the file descriptor
                SharedBuffer<std::byte> ptr(fd);

                BufferPtr t = BufferPtr::create_with(std::move(ptr));
                return t;
            }
            return cg_value();
        }

        cg_value unpack_value()
        {
            if (maybe(kNone))
            {
                return cg_value();
            }

            if (maybe(kUInt8))
            {
                return read_value<uint8_t>();
            }

            if (maybe(kInt8))
            {
                return read_value<int8_t>();
            }

            if (maybe(kUInt16))
            {
                return read_value<uint16_t>();
            }

            if (maybe(kInt16))
            {
                return read_value<int16_t>();
            }

            if (maybe(kUInt32))
            {
                return read_value<uint32_t>();
            }

            if (maybe(kInt32))
            {
                return read_value<int32_t>();
            }

            if (maybe(kFloat))
            {
                return read_value<float>();
            }

            if (maybe(kDouble))
            {
                return read_value<double>();
            }

            if (maybe(kAny))
            {
                uint8_t buf_type = read_value<uint8_t>();
                if (buf_type == kCopyBuffer)
                {
                    uint32_t buf_size = read_value<uint32_t>();
                    UniquePtr<std::byte> buf = make_raw_buffer(buf_size);
                    BufferPtr bufptr = BufferPtr::create_with((uint32_t)buf_size, std::move(buf));
                    return bufptr;
                }
                // It is shared memory buffer. Not related to normal buffer
                // shared between nodes
                else if (buf_type == kSharedBuffer)
                {
                    uint32_t buf_size = read_value<uint32_t>();
                    int32_t global_id = read_value<int32_t>();
                    if (!buf_size)
                    {
                        return cg_value();
                    }

                    if (MemServer::mem_server == nullptr)
                    {
                        return cg_value();
                    }

                    Descriptor *fd = MemServer::mem_server->get_buffer(global_id);
                    if (fd == nullptr)
                    {
                        return cg_value();
                    }

                    MemServer::mem_server->release(global_id);
                    
                    SharedBuffer<std::byte> ptr(fd);
                    BufferPtr t = BufferPtr::create_with(std::move(ptr));
                    return t;
                }
                return cg_value();
            }

            if (maybe(kStr))
            {
                uint32_t str_size = read_value<uint32_t>();
                std::shared_ptr<char> buf = make_string_buffer(str_size);
                return (std::string(buf.get(), str_size - 1)); // Exclude null terminator
            }

            if (maybe(kTensor))
            {
                uint8_t nb_dims = read_value<uint8_t>();
                cg_tensor_dims_t dims;
                read_array<uint32_t, CG_TENSOR_NB_DIMS>(dims);
                if (maybe(kInt8))
                {
                    return (unpack_tensor<int8_t>(nb_dims, dims));
                }
                else if (maybe(kInt16))
                {
                    return (unpack_tensor<int16_t>(nb_dims, dims));
                }
                else if (maybe(kInt32))
                {
                    return (unpack_tensor<int32_t>(nb_dims, dims));
                }
                else if (maybe(kInt64))
                {
                    return (unpack_tensor<int64_t>(nb_dims, dims));
                }
                else if (maybe(kUInt8))
                {
                    return (unpack_tensor<uint8_t>(nb_dims, dims));
                }
                else if (maybe(kUInt16))
                {
                    return (unpack_tensor<uint16_t>(nb_dims, dims));
                }
                else if (maybe(kUInt32))
                {
                    return (unpack_tensor<uint32_t>(nb_dims, dims));
                }
                else if (maybe(kUInt64))
                {
                    return (unpack_tensor<uint64_t>(nb_dims, dims));
                }
                else if (maybe(kFloat))
                {
                    return (unpack_tensor<float>(nb_dims, dims));
                }
                else if (maybe(kDouble))
                {
                    return (unpack_tensor<double>(nb_dims, dims));
                }
                else
                {
                    return (cg_value());
                }
            }

            return (cg_value());
        }

    private:
        const uint8_t *data_pointer = nullptr;
    };

};
