/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        StreamNode.h
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

#pragma once

#include <memory>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <variant>
#include <array>
#include <string>

#include "cg_enums.h"

#define CG_TENSOR_NB_DIMS 8

#ifndef CG_MK_LIST_EVENT_ALLOCATOR
#define CG_MK_LIST_EVENT_ALLOCATOR(T) std::allocator<T>{}
#endif

#ifndef CG_PROTECTED_BUF_ALLOCATOR_TYPE
#define CG_PROTECTED_BUF_ALLOCATOR_TYPE(T) std::allocator<T>
#endif

#ifndef CG_MK_PROTECTED_BUF_ALLOCATOR
#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) std::allocator<T>{}
#endif

// We create a compilation error is used and not defined
// the compilation error will occur only when built with
// CG_EVENTS_MULTI_THREAD
#ifndef CG_MUTEX
#define CG_MUTEX void
#endif

#ifndef CG_INIT_MUTEX
#define CG_INIT_MUTEX(MUTEX)
#endif

#ifndef CG_MUTEX_DELETE
#define CG_MUTEX_DELETE(MUTEX, ERROR)
#endif

#ifndef CG_MUTEX_ERROR_TYPE
#define CG_MUTEX_ERROR_TYPE int
#endif

#ifndef CG_MUTEX_HAS_ERROR
#define CG_MUTEX_HAS_ERROR(ERROR) false
#endif

#ifndef CG_ENTER_CRITICAL_SECTION
#define CG_ENTER_CRITICAL_SECTION(mutex, error)
#endif

#ifndef CG_EXIT_CRITICAL_SECTION
#define CG_EXIT_CRITICAL_SECTION(mutex, error)
#endif

#ifndef CG_ENTER_READ_CRITICAL_SECTION
#define CG_ENTER_READ_CRITICAL_SECTION(mutex, error)
#endif

#ifndef CG_EXIT_READ_CRITICAL_SECTION
#define CG_EXIT_READ_CRITICAL_SECTION(mutex, error)
#endif

// Maximum number of values in a combined value
#ifndef CG_MAX_VALUES
#define CG_MAX_VALUES 8
#endif

/* Node ID is -1 when nodes are not identified for the external
world */
#define CG_UNIDENTIFIED_NODE (-1)

namespace arm_cmsis_stream
{
    template <typename T, void (*Fn)(T *)>
    struct DeleterThunk
    {
        static void call(void *p)
        {
            Fn(static_cast<T *>(p));
        }
    };

    template <typename T>
    class UniquePtr
    {
    public:
        using deleter_t = void (*)(void *);

        UniquePtr() = default;

        explicit UniquePtr(size_t nb) : deleter_(DefaultDeleter<T>::delete_ptr)
        {
            ptr_ = std::malloc(nb * sizeof(T));
        }

        template <typename U>
        explicit UniquePtr(U *ptr) noexcept
            : ptr_(const_cast<std::remove_const_t<U> *>(ptr)), deleter_(DefaultDeleter<U>::delete_ptr)
        {
        }

        template <typename D>
        explicit UniquePtr(T *ptr, D deleter) noexcept
            : ptr_(ptr)
        {

            constexpr auto fn = static_cast<void (*)(T *)>(+deleter);

            // Declare the thunk type
            using Thunk = DeleterThunk<T, fn>;

            deleter_ = &Thunk::call;
        }

        UniquePtr(const UniquePtr &other) = delete; // Disable copy constructor
        UniquePtr(UniquePtr &&other) noexcept
            : ptr_(other.ptr_),
              deleter_(other.deleter_)
        {
            other.ptr_ = nullptr;     // Reset the moved-from pointer
            other.deleter_ = nullptr; // Reset the moved-from deleter
        }

        UniquePtr &operator=(const UniquePtr &other) = delete; // Disable copy assignment
        UniquePtr &operator=(UniquePtr &&other) noexcept
        {
            if (this != &other)
            {
                reset();
                ptr_ = other.ptr_;
                deleter_ = other.deleter_;
                other.ptr_ = nullptr;     // Reset the moved-from pointer
                other.deleter_ = nullptr; // Reset the moved-from deleter
            }
            return *this;
        }

        ~UniquePtr()
        {
            reset();
        }

        bool operator==(std::nullptr_t) const noexcept { return ptr_ == nullptr; }
        bool operator!=(std::nullptr_t) const noexcept { return ptr_ != nullptr; }

        UniquePtr &operator=(std::nullptr_t) noexcept
        {
            reset();      // delete existing object if any
            return *this; // pointer is now null
        }

        void reset() noexcept
        {
            if (ptr_)
            {
                deleter_(ptr_);
                ptr_ = nullptr;
            }
        }

        T *get() const noexcept { return static_cast<T *>(ptr_); }

        template <typename R>
        R *as() noexcept { return static_cast<R *>(ptr_); }

        template <typename R>
        const R *as() const noexcept { return static_cast<const R *>(ptr_); }

        T *operator->() { return static_cast<T *>(ptr_); }

        T &operator*() { return *(static_cast<T *>(ptr_)); }

        T &operator[](std::size_t i) { return (static_cast<T *>(ptr_))[i]; }

        explicit operator bool() const noexcept { return ptr_ != nullptr; }

    private:
        // Default deleters
        template <typename U>
        struct DefaultDeleter
        {
            static void delete_ptr(void *p)
            {
                std::free(p);
            }
        };

        template <typename U>
        struct DefaultDeleter<const U>
        {
            static void delete_ptr(void *)
            {
                // no-op for const U*
            }
        };

        void *ptr_ = nullptr;
        deleter_t deleter_ = nullptr;
    };

    template <typename T>
    class ProtectedBuffer
    {
    public:
        using value_type = T;
        using allocator_type = CG_PROTECTED_BUF_ALLOCATOR_TYPE(T);

    private:
        struct ControlBlock
        {
            T object;
#if defined(CG_EVENTS_MULTI_THREAD)
            mutable CG_MUTEX mutex;
#endif
            template <typename... Args>
            ControlBlock(Args &&...args) : object{std::forward<Args>(args)...}
            {
                CG_INIT_MUTEX(mutex);
            }

            ~ControlBlock()
            {
                CG_MUTEX_ERROR_TYPE error;
                CG_MUTEX_DELETE(mutex, error);
            }
        };

        using BlockAllocator = typename std::allocator_traits<allocator_type>::template rebind_alloc<ControlBlock>;
        std::shared_ptr<ControlBlock> control;

    public:
        // Create with custom allocator and arguments for T
        template <typename... Args>
        static ProtectedBuffer create_with(Args &&...args)
        {
            BlockAllocator block_alloc(CG_MK_PROTECTED_BUF_ALLOCATOR(T));
            auto ptr = std::allocate_shared<typename ControlBlock>(block_alloc, std::forward<Args>(args)...);
            return ProtectedBuffer(ptr);
        }

        // Exclusive (write) access
        template <typename Func>
        auto lock(Func &&f) -> decltype(f(CG_MUTEX_ERROR_TYPE(), std::declval<T &>()))
        {
            using R = decltype(f(CG_MUTEX_ERROR_TYPE(), std::declval<T &>()));
            if constexpr (std::is_void_v<R>)
            {
                CG_MUTEX_ERROR_TYPE error;
                CG_ENTER_CRITICAL_SECTION(control->mutex, error);

                f(error, control->object);

                if (!CG_MUTEX_HAS_ERROR(error))
                {
                    CG_EXIT_CRITICAL_SECTION(control->mutex, error);
                }

                return;
            }
            else
            {
                R r{};
                CG_MUTEX_ERROR_TYPE error;
                CG_ENTER_CRITICAL_SECTION(control->mutex, error);

                r = f(error, control->object);

                if (!CG_MUTEX_HAS_ERROR(error))
                {
                    CG_EXIT_CRITICAL_SECTION(control->mutex, error);
                }
                return r;
            }
        }

        // Shared (read) access
        template <typename Func>
        auto lock_shared(Func &&f) const -> decltype(f(CG_MUTEX_ERROR_TYPE(), std::declval<const T &>()))
        {
            using R = decltype(f(CG_MUTEX_ERROR_TYPE(), std::declval<const T &>()));
            if constexpr (std::is_void_v<R>)
            {
                CG_MUTEX_ERROR_TYPE error;
                CG_ENTER_READ_CRITICAL_SECTION(control->mutex, error);

                f(error, control->object);

                if (!CG_MUTEX_HAS_ERROR(error))
                {
                    CG_EXIT_READ_CRITICAL_SECTION(control->mutex, error);
                }
                return;
            }
            else
            {
                R r{};
                CG_MUTEX_ERROR_TYPE error;
                CG_ENTER_READ_CRITICAL_SECTION(control->mutex, error);

                r = f(error, control->object);

                if (!CG_MUTEX_HAS_ERROR(error))
                {
                    CG_EXIT_CRITICAL_SECTION(control->mutex, error);
                }
                return r;
            }
        }

        // Copyable and shareable
        ProtectedBuffer() = default;
        ProtectedBuffer(const ProtectedBuffer &) = default;
        ProtectedBuffer(ProtectedBuffer &&) noexcept = default;
        ProtectedBuffer &operator=(const ProtectedBuffer &) = default;
        ProtectedBuffer &operator=(ProtectedBuffer &&) noexcept = default;

    private:
        explicit ProtectedBuffer(std::shared_ptr<ControlBlock> c) : control(std::move(c)) {}
    };

    /* Made to be used from a ProtectedBuffer and contained in a
    shared_ptr. The RawBuffer in itself must be unique. */
    struct RawBuffer
    {
        uint32_t buf_size;
        UniquePtr<std::byte> data;

        explicit RawBuffer(uint32_t size, UniquePtr<std::byte> &&buf_data) noexcept : buf_size(size), data(std::move(buf_data))
        {
            if (data == nullptr)
            {
                buf_size = 0; // Reset size if data is null
            }
        }
        RawBuffer(const RawBuffer &other) = delete;
        RawBuffer(RawBuffer &&other) noexcept : buf_size(other.buf_size), data(std::move(other.data))
        {
            other.buf_size = 0;   // Reset the moved-from object
            other.data = nullptr; // Reset the moved-from object
        }
        RawBuffer &operator=(const RawBuffer &other) = delete;

        RawBuffer &operator=(RawBuffer &&other) noexcept
        {
            if (this != &other)
            {
                buf_size = other.buf_size;
                data = std::move(other.data);
                other.buf_size = 0;   // Reset the moved-from object
                other.data = nullptr; // Reset the moved-from object
            }
            return *this;
        }
    };

    using cg_tensor_dims_t = std::array<uint32_t, CG_TENSOR_NB_DIMS>;

    template <typename T>
    struct Tensor
    {
        uint8_t nb_dims; // Maximum CG_TENSOR_NB_DIMS dimensions
        // rows, columns etc ..
        cg_tensor_dims_t dims;
        // Data is columns then row etc ...
        UniquePtr<T> data;

        explicit Tensor(uint8_t dimensions,
                        const cg_tensor_dims_t &dimensions_array,
                        UniquePtr<T> tensor_data) noexcept
            : nb_dims(dimensions), dims(dimensions_array), data(std::move(tensor_data))
        {
            if (data == nullptr)
            {
                nb_dims = 0; // Reset dimensions if data is null or size is zero
            }
        }

        Tensor(const Tensor &other) = delete;

        Tensor(Tensor &&other) noexcept
            : nb_dims(other.nb_dims), dims(other.dims), data(std::move(other.data))
        {
            other.nb_dims = 0;    // Reset the moved-from object
            other.data = nullptr; // Reset the moved-from object
        }

        Tensor &operator=(const Tensor &other) = delete;
        Tensor &operator=(Tensor &&other) noexcept
        {
            if (this != &other)
            {
                nb_dims = other.nb_dims;
                dims = other.dims;
                data = std::move(other.data);
                other.nb_dims = 0;    // Reset the moved-from object
                other.data = nullptr; // Reset the moved-from object
            }
            return *this;
        }

        size_t size() const noexcept
        {
            if (nb_dims == 0)
            {
                return 0;
            }

            uint32_t total_size = 1;
            for (uint8_t i = 0; i < nb_dims; ++i)
            {
                total_size *= dims[i]; // Calculate total size based on dimensions
            }
            return total_size;
        }
    };

    template <typename T>
    using TensorPtr = ProtectedBuffer<Tensor<T>>;

    using BufferPtr = ProtectedBuffer<RawBuffer>;

    using cg_any_tensor = std::variant<TensorPtr<uint8_t>,
                                       TensorPtr<int8_t>,
                                       TensorPtr<uint16_t>,
                                       TensorPtr<int16_t>,
                                       TensorPtr<uint32_t>,
                                       TensorPtr<int32_t>,
                                       TensorPtr<uint64_t>,
                                       TensorPtr<int64_t>,
                                       TensorPtr<float>,
                                       TensorPtr<double>>;

    using cg_any_const_tensor = std::variant<TensorPtr<const uint8_t>,
                                             TensorPtr<const int8_t>,
                                             TensorPtr<const uint16_t>,
                                             TensorPtr<const int16_t>,
                                             TensorPtr<const uint32_t>,
                                             TensorPtr<const int32_t>,
                                             TensorPtr<const uint64_t>,
                                             TensorPtr<const int64_t>,
                                             TensorPtr<const float>,
                                             TensorPtr<const double>>;

    using cg_value_variant = std::variant<std::monostate,
                                          int8_t,
                                          int16_t,
                                          int32_t,
                                          int64_t,
                                          float,
                                          double,
                                          uint8_t,
                                          uint16_t,
                                          uint32_t,
                                          uint64_t,
                                          BufferPtr,
                                          std::string,
                                          cg_any_tensor,
                                          cg_any_const_tensor>;

    struct cg_value
    {

        cg_value() noexcept
        {
            value = std::monostate(); // Initialize to an empty state
        }

        ~cg_value()
        {
        }

        cg_value(const cg_value &other) noexcept
        {
            copyFrom(other); // Copy the value from other
        }

        cg_value(cg_value &&other) noexcept
        {
            moveFrom(std::move(other)); // Move the value from other
        }

        cg_value &operator=(const cg_value &other) noexcept
        {
            if (this != &other)
            {
                copyFrom(other); // Copy the value from other
            }
            return *this; // Return the current object
        }

        cg_value &operator=(cg_value &&other) noexcept
        {
            if (this != &other)
            {
                moveFrom(std::move(other)); // Move the value from other
            }
            return *this; // Return the current object
        }

        cg_value(int8_t v) noexcept : value(v) {};

        cg_value(int16_t v) noexcept : value(v) {};

        cg_value(int32_t v) noexcept : value(v) {};

        cg_value(int64_t v) noexcept : value(v) {};

        cg_value(float v) noexcept : value(v) {};

        cg_value(double v) noexcept : value(v) {};

        cg_value(uint8_t v) noexcept : value(v) {};

        cg_value(uint16_t v) noexcept : value(v) {};

        cg_value(uint32_t v) noexcept : value(v) {};

        cg_value(uint64_t v) noexcept : value(v) {};

        cg_value(const std::string &s) noexcept : value(s) {};
        cg_value(std::string &&s) noexcept : value(std::move(s)) {};

        template <typename T>
        cg_value(const TensorPtr<T> &t) noexcept : value(t){};

        template <typename T>
        cg_value(TensorPtr<T> &&t) noexcept : value(std::move(t)){};

        cg_value(const BufferPtr &t) noexcept : value(t) {};

        cg_value(BufferPtr &&t) noexcept : value(std::move(t)) {};

        cg_value_variant value;

    protected:
        void moveFrom(cg_value &&other) noexcept
        {
            value = std::move(other.value); // Move the value
            other.value = std::monostate(); // Reset the moved-from value
        }

        void copyFrom(const cg_value &other) noexcept
        {
            value = other.value;
        }
    };

    struct ListValue
    {
        uint32_t nb_values = 0;                     // Number of values in the combined value
        std::array<cg_value, CG_MAX_VALUES> values; // Array of values
    };

    using EventData = std::variant<cg_value, std::shared_ptr<ListValue>>;

    template <typename T>
    struct ValueParse
    {
        static inline bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<T>(data.value);
        }

        static inline T getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<T>(data.value))
            {
                return std::get<T>(data.value);
            }
            return T(); // Default value if not found
        }
    };

    template <typename T>
    struct ValueParse<TensorPtr<T>>
    {
        static bool contains(const cg_value &data) noexcept
        {
            if constexpr (std::is_const<T>::value)
            {
                if (std::holds_alternative<cg_any_const_tensor>(data.value))
                {
                    const cg_any_const_tensor &tensor = std::get<cg_any_const_tensor>(data.value);
                    return (std::holds_alternative<TensorPtr<T>>(tensor));
                }
            }
            else
            {
                if (std::holds_alternative<cg_any_tensor>(data.value))
                {
                    const cg_any_tensor &tensor = std::get<cg_any_tensor>(data.value);
                    return (std::holds_alternative<TensorPtr<T>>(tensor));
                }
            }

            return false;
        }

        static inline TensorPtr<T> getValue(const cg_value &data) noexcept
        {
            if constexpr (std::is_const<T>::value)
            {

                if (std::holds_alternative<cg_any_const_tensor>(data.value))
                {
                    const cg_any_const_tensor &tensor = std::get<cg_any_const_tensor>(data.value);
                    if (std::holds_alternative<TensorPtr<T>>(tensor))
                    {
                        return std::get<TensorPtr<T>>(tensor);
                    }
                }
            }
            else
            {
                if (std::holds_alternative<cg_any_tensor>(data.value))
                {
                    const cg_any_tensor &tensor = std::get<cg_any_tensor>(data.value);
                    if (std::holds_alternative<TensorPtr<T>>(tensor))
                    {
                        return std::get<TensorPtr<T>>(tensor);
                    }
                }
            }

            return TensorPtr<T>();
        }
    };

    template <>
    struct ValueParse<uint16_t>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<uint16_t>(data.value) || ValueParse<uint8_t>::contains(data);
        }

        static inline uint16_t getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<uint16_t>(data.value))
            {
                return std::get<uint16_t>(data.value);
            }
            else
            {
                return static_cast<uint16_t>(ValueParse<uint8_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<uint32_t>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<uint32_t>(data.value) || ValueParse<uint16_t>::contains(data);
        }

        static inline uint32_t getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<uint32_t>(data.value))
            {
                return std::get<uint32_t>(data.value);
            }
            else
            {
                return static_cast<uint32_t>(ValueParse<uint16_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<uint64_t>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<uint64_t>(data.value) || ValueParse<uint32_t>::contains(data);
        }

        static inline uint64_t getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<uint64_t>(data.value))
            {
                return std::get<uint64_t>(data.value);
            }
            else
            {
                return static_cast<uint64_t>(ValueParse<uint32_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<int16_t>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<int16_t>(data.value) || ValueParse<int8_t>::contains(data);
        }

        static inline int16_t getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<int16_t>(data.value))
            {
                return std::get<int16_t>(data.value);
            }
            else
            {
                return static_cast<int16_t>(ValueParse<int8_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<int32_t>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<int32_t>(data.value) || ValueParse<int16_t>::contains(data);
        }

        static inline int32_t getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<int32_t>(data.value))
            {
                return std::get<int32_t>(data.value);
            }
            else
            {
                return static_cast<int32_t>(ValueParse<int16_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<int64_t>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<int64_t>(data.value) || ValueParse<int32_t>::contains(data);
        }

        static inline int64_t getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<int64_t>(data.value))
            {
                return std::get<int64_t>(data.value);
            }
            else
            {
                return static_cast<int64_t>(ValueParse<int32_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<float>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<float>(data.value) ||
                   ValueParse<int32_t>::contains(data);
        }

        static inline float getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<float>(data.value))
            {
                return std::get<float>(data.value);
            }
            else
            {
                return static_cast<float>(ValueParse<int32_t>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    template <>
    struct ValueParse<double>
    {
        static bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<double>(data.value) ||
                   std::holds_alternative<int64_t>(data.value) ||
                   ValueParse<float>::contains(data);
        }

        static inline double getValue(const cg_value &data) noexcept
        {
            if (std::holds_alternative<double>(data.value))
            {
                return std::get<double>(data.value);
            }
            else if (std::holds_alternative<int64_t>(data.value))
            {
                return static_cast<double>(std::get<int64_t>(data.value));
            }
            else
            {
                return static_cast<double>(ValueParse<float>::getValue(data)); // Fallback to uint32_t if not found
            }
        }
    };

    class Event
    {
    protected:

        

        void setPriority(enum cg_event_priority evtPriority) noexcept
        {
            switch (evtPriority)
            {
            case kLowPriority:
                priority = 0;
                break;
            case kNormalPriority:
                priority = 1;
                break;
            case kHighPriority:
                priority = 2;
                break;
            default:
                priority = 1; // Default to normal priority
            }
        }
        void moveFrom(Event &&other) noexcept
        {
            event_id = other.event_id;
            data = std::move(other.data);
            priority = other.priority;
            other.event_id = kNoEvent;
        }

        void copyFrom(const Event &other) noexcept
        {
            event_id = other.event_id;
            data = other.data;
            priority = other.priority;
        }

        template <typename... Args, std::size_t... Is>
        bool check_array_types(const std::array<cg_value, CG_MAX_VALUES> &values,
                               std::index_sequence<Is...>) const noexcept
        {
            return (... && ValueParse<
                               typename std::tuple_element<Is, std::tuple<Args...>>::type>::contains(values[Is]));
        };

        template <typename F, typename O, typename... Args, std::size_t... Is>
        void apply_array_types(F &&f, O &&o, const std::array<cg_value, CG_MAX_VALUES> &values,
                               std::index_sequence<Is...>) const
        {
            (o.*f)(ValueParse<
                   typename std::tuple_element<Is, std::tuple<Args...>>::type>::getValue(values[Is])...);
        };

    public:
        uint32_t event_id;
        uint32_t priority;
        EventData data;

        static std::shared_ptr<ListValue> make_new_list_value()
        {
            return std::allocate_shared<ListValue>(CG_MK_LIST_EVENT_ALLOCATOR(ListValue));
        }

        Event() noexcept : event_id(kNoEvent), priority(kNormalPriority)
        {
            data = cg_value();
        }

        Event(uint32_t id,
              std::shared_ptr<ListValue> cv,
              enum cg_event_priority evtPriority) noexcept : event_id(id)
        {
            data = std::move(cv);
            setPriority(evtPriority);
        };

        template <typename... Args>
        Event(uint32_t selector,
              enum cg_event_priority evtPriority,
              Args &&...args) : event_id(selector)
        {
            static_assert(sizeof...(Args) <= CG_MAX_VALUES, "Too many arguments for combined value");

            setPriority(evtPriority);

            if constexpr (sizeof...(Args) == 0)
            {
                data = cg_value();
            }
            else if constexpr (sizeof...(Args) == 1)
            {
                data = cg_value(std::get<0>(std::forward_as_tuple(std::forward<Args>(args)...)));
            }
            else if constexpr (sizeof...(Args) > 1)
            {
                std::shared_ptr<ListValue> cbv = make_new_list_value();
                if (cbv)
                {
                    cbv->nb_values = sizeof...(Args);
                    size_t i = 0;
                    ((cbv->values[i++] = cg_value(std::forward<Args>(args))), ...);
                    data = std::move(cbv);
                }
                else
                {
                    event_id = kNoEvent;
                    data = cg_value();
                }
            }
        };

        Event(const Event &other) noexcept
        {
            copyFrom(other);
        }

        Event(Event &&other) noexcept
        {
            moveFrom(std::move(other));
        }

        Event &operator=(const Event &other) noexcept
        {
            if (this != &other)
            {
                copyFrom(other);
            }
            return *this;
        }

        Event &operator=(Event &&other) noexcept
        {
            if (this != &other)
            {
                moveFrom(std::move(other));
            }
            return *this;
        }

        template <typename T>
        bool contains() const noexcept
        {
            if (std::holds_alternative<cg_value>(data))
            {
                const cg_value &val = std::get<cg_value>(data);
                return ValueParse<T>::contains(val);
            }
            return false;
        }

        template <typename T>
        T get() const noexcept
        {
            if (std::holds_alternative<cg_value>(data))
            {
                const cg_value &val = std::get<cg_value>(data);

                return ValueParse<T>::getValue(val);
            }
            return T{}; // Default value if not found
        }

        template <typename... Args>
        bool wellFormed() const noexcept
        {
            if constexpr (sizeof...(Args) == 0)
            {
                return false; // No arguments provided
            }
            if constexpr (sizeof...(Args) == 1)
            {
                if (std::holds_alternative<cg_value>(data))
                {
                    const cg_value &val = std::get<cg_value>(data);
                    return ValueParse<Args...>::contains(val); // Check if the single argument matches
                }
                else
                {
                    return false;
                }
            }
            else if constexpr (sizeof...(Args) > 1)
            {

                if (std::holds_alternative<std::shared_ptr<ListValue>>(data))
                {
                    std::shared_ptr<ListValue> cbv = std::get<std::shared_ptr<ListValue>>(data);
                    if (cbv && cbv->nb_values > 0)
                    {
                        if (sizeof...(Args) != cbv->nb_values)
                        {
                            return false; // Number of arguments does not match
                        }
                        else
                        {
                            return (check_array_types<Args...>(cbv->values,
                                                               std::make_index_sequence<sizeof...(Args)>{}));
                        }
                    }
                }
                else
                {
                    return false; // Data is not a combined value
                }
            }
            return false;
        };

        template <typename... Args, typename F, typename O>
        bool apply(F &&f, O &&o) const
        {
            if constexpr (sizeof...(Args) == 0)
            {
                return false; // No arguments provided
            }
            if constexpr (sizeof...(Args) == 1)
            {
                if (std::holds_alternative<cg_value>(data))
                {
                    const cg_value &val = std::get<cg_value>(data);
                    (o.*f)(ValueParse<Args...>::getValue(val)); // Check if the single argument matches
                    return true;
                }
                else
                {

                    return false;
                }
            }
            else if constexpr (sizeof...(Args) > 1)
            {

                if (std::holds_alternative<std::shared_ptr<ListValue>>(data))
                {

                    std::shared_ptr<ListValue> cbv = std::get<std::shared_ptr<ListValue>>(data);
                    if (cbv && cbv->nb_values > 0)
                    {
                        if (sizeof...(Args) != cbv->nb_values)
                        {

                            return false; // Number of arguments does not match
                        }
                        else
                        {
                            apply_array_types<F, O, Args...>(std::forward<F>(f), std::forward<O>(o), cbv->values,
                                                             std::make_index_sequence<sizeof...(Args)>{});
                            return true;
                        }
                    }
                    else
                    {
                        return false; // Data is not a combined value
                    }
                }
                else
                {

                    return false; // Data is not a combined value
                }
            }
            return false;
        };
    };

    /* CMSIS Stream Node : event and streaming inherit from this class */
    class StreamNode
    {
    public:
        virtual ~StreamNode() {};

        StreamNode() {};

        virtual void processEvent(int dstPort, const Event &evt) {};

        /*
        Nodes are fixed and not made to be copied or moved.
        */
        StreamNode(const StreamNode &) = delete;
        StreamNode(StreamNode &&) = delete;
        StreamNode &operator=(const StreamNode &) = delete;
        StreamNode &operator=(StreamNode &&) = delete;

        void setID(int id) noexcept { mNodeID = id; };
        int nodeID() const noexcept { return (mNodeID); };

    private:
        int mNodeID = CG_UNIDENTIFIED_NODE;
    };
}; // end namespace
