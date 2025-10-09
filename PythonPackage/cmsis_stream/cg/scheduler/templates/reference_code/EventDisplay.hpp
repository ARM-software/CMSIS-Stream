#pragma once

#include <cstdint>
#include <iostream>
#include <variant>
#include "StreamNode.hpp"

using namespace arm_cmsis_stream;

template <typename T>
void disp_array(std::ostream &os, const T *obj, std::size_t nb)
{
    std::size_t k = 0;
    for (std::size_t i = 0; i < nb; i++)
    {
        if (k == 10)
        {
            os << std::endl;
            k = 0;
        }

        os << obj[i] << " ";

        k++;
    }
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const TensorPtr<T> &obj)
{
    bool lockError;
    obj.lock_shared(lockError,[&os](const Tensor<T> &t)
                    {

            os << "Tensor[";
            for (uint8_t i = 0; i < t.nb_dims; ++i)
            {
                os << t.dims[i];
                if (i < t.nb_dims - 1)
                    os << ", ";
            }
            os << "]"; 
            if (std::holds_alternative<UniquePtr<T>>(t.data))
            {
                const UniquePtr<T> &buf = std::get<UniquePtr<T>>(t.data);
                disp_array(os, buf.get(), t.size()); 
            }
            else
            {
                os << "Shared memory";
            }
        });
           
    return os;
}

std::ostream &operator<<(std::ostream &os, const cg_any_tensor &obj)
{
    if (std::holds_alternative<TensorPtr<uint8_t>>(obj))
    {
        os << "uint8_t ";
        auto tensor = std::get<TensorPtr<uint8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<uint16_t>>(obj))
    {
        os << "uint16_t ";
        auto tensor = std::get<TensorPtr<uint16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<uint32_t>>(obj))
    {
        os << "uint32_t ";
        auto tensor = std::get<TensorPtr<uint32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<uint64_t>>(obj))
    {
        os << "uint64_t ";
        auto tensor = std::get<TensorPtr<uint64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int8_t>>(obj))
    {
        os << "int8_t ";
        auto tensor = std::get<TensorPtr<int8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int16_t>>(obj))
    {
        os << "int16_t ";
        auto tensor = std::get<TensorPtr<int16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int32_t>>(obj))
    {
        os << "int32_t ";
        auto tensor = std::get<TensorPtr<int32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int64_t>>(obj))
    {
        os << "int64_t ";
        auto tensor = std::get<TensorPtr<int64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<double>>(obj))
    {
        os << "double ";
        auto tensor = std::get<TensorPtr<double>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<float>>(obj))
    {
        os << "float ";
        auto tensor = std::get<TensorPtr<float>>(obj);
        os << tensor;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const cg_any_const_tensor &obj)
{
    if (std::holds_alternative<TensorPtr<const uint8_t>>(obj))
    {
        os << "const uint8_t ";
        auto tensor = std::get<TensorPtr<const uint8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const uint16_t>>(obj))
    {
        os << "const uint16_t ";
        auto tensor = std::get<TensorPtr<const uint16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const uint32_t>>(obj))
    {
        os << "const uint32_t ";
        auto tensor = std::get<TensorPtr<const uint32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const uint64_t>>(obj))
    {
        os << "const uint64_t ";
        auto tensor = std::get<TensorPtr<const uint64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int8_t>>(obj))
    {
        os << "const int8_t ";
        auto tensor = std::get<TensorPtr<const int8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int16_t>>(obj))
    {
        os << "const int16_t ";
        auto tensor = std::get<TensorPtr<const int16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int32_t>>(obj))
    {
        os << "const int32_t ";
        auto tensor = std::get<TensorPtr<const int32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int64_t>>(obj))
    {
        os << "const int64_t ";
        auto tensor = std::get<TensorPtr<const int64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const double>>(obj))
    {
        os << "const double ";
        auto tensor = std::get<TensorPtr<const double>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const float>>(obj))
    {
        os << "const float ";
        auto tensor = std::get<TensorPtr<const float>>(obj);
        os << tensor;
    }
    return os;
}


// Overload operator<<
std::ostream &operator<<(std::ostream &os, const cg_value &obj)
{
    if (std::holds_alternative<int8_t>(obj.value))
    {
        os << std::get<int8_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<int16_t>(obj.value))
    {
        os << std::get<int16_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<int32_t>(obj.value))
    {
        os << std::get<int32_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<int64_t>(obj.value))
    {
        os << std::get<int64_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint8_t>(obj.value))
    {
        os << std::get<uint8_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint16_t>(obj.value))
    {
        os << std::get<uint16_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint32_t>(obj.value))
    {
        os << std::get<uint32_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint64_t>(obj.value))
    {
        os << std::get<uint64_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<float>(obj.value))
    {
        os << std::get<float>(obj.value);
        return os;
    }

    if (std::holds_alternative<double>(obj.value))
    {
        os << std::get<double>(obj.value);
        return os;
    }

    if (std::holds_alternative<BufferPtr>(obj.value))
    {
        bool lockError;
        std::get<BufferPtr>(obj.value).lock_shared(lockError,[&os]( const RawBuffer &buf)
                                                   { os << "Buffer(size=" << buf.buf_size << ")"; });
        return os;
    }

    if (std::holds_alternative<std::string>(obj.value))
    {
        os << std::get<std::string>(obj.value);
        return os;
    }

    if (std::holds_alternative<cg_any_tensor>(obj.value))
    {
        cg_any_tensor anyt = std::get<cg_any_tensor>(obj.value);
        os << anyt;
    }

    if (std::holds_alternative<cg_any_const_tensor>(obj.value))
    {
        cg_any_const_tensor anyt = std::get<cg_any_const_tensor>(obj.value);
        os << anyt;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const ListValue &obj)
{
    os << "CombinedValue: "
       << "nb_values=" << obj.nb_values;
    for (uint32_t i = 0; i < obj.nb_values; ++i)
    {
        os << ", value[" << i << "]=" << obj.values[i];
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, Event obj)
{
    os << "Event: event_id=" << obj.event_id
       << ", priority=" << obj.priority;
    if (std::holds_alternative<cg_value>(obj.data))
    {
        os << ", data=" << std::get<cg_value>(obj.data);
    }
    else if (std::holds_alternative<UniquePtr<ListValue>>(obj.data))
    {
        os << ", combined_data=" << *std::get<UniquePtr<ListValue>>(obj.data).get();
    }
    else
    {
        os << ", data=unknown";
    }
    return os;
}