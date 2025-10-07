/* ----------------------------------------------------------------------
 * Project:      CMSIS Stream Library
 * Title:        StreamNode.hpp
 * Description:  Main classes and templates for CMSIS-Stream nodes.
 *               All nodes must inherit from StreamNode.
 *               Dataflow nodes must inherit from NodeBase.
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

// Used for ListValue
#ifndef CG_MK_LIST_EVENT_ALLOCATOR
#define CG_MK_LIST_EVENT_ALLOCATOR(T) \
    std::allocator<T> {}
#endif

// Used for RawBuffer and Tensors
#ifndef CG_MK_PROTECTED_BUF_ALLOCATOR
#define CG_MK_PROTECTED_BUF_ALLOCATOR(T) \
    std::allocator<T> {}
#endif

// Used for mutexes used to protect access to the buffers
// since they are allocated in a shared_ptr
#ifndef CG_MK_PROTECTED_MUTEX_ALLOCATOR
#define CG_MK_PROTECTED_MUTEX_ALLOCATOR(T) \
    std::allocator<T> {}
#endif

// Used only when CG_EVENTS_MULTI_THREAD is ON
// but a definition is needed to build
#ifndef CG_MUTEX
#define CG_MUTEX int
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

/* Node ID is -1 when nodes are not identified for the external world */
#define CG_UNIDENTIFIED_NODE (-1)

namespace arm_cmsis_stream
{
    
    class Descriptor;

    using NativeHandle =
#ifdef _WIN32
        void *;
#else
        int;
#endif

    class MemServer
    {
    public:
        virtual ~MemServer() {};
        inline static MemServer *mem_server = nullptr;

        // Write locks
        // Changes to refcount are blocked while a write lock
        // is held
        virtual int lock(int32_t offset) const = 0;
        virtual int unlock(int32_t offset) const = 0;

        // Read locks
        virtual int read_lock(int32_t offset) const = 0;
        virtual int read_unlock(int32_t offset) const = 0;

        // Acquire the buffer
        virtual int acquire(int32_t offset) const = 0;
        // Release the buffer when no more needed by a process
        virtual int release(int32_t offset) const = 0;

        // This function is not locking
        // If you want to prevent the refcount change,
        // you should use the ipc_lock that locks the full table
        virtual int32_t refcount(int32_t offset) const = 0;

        // Buffer has refcount 1 after creation
        virtual Descriptor *new_buffer(size_t size) const = 0;
        // Get buffer increases the refcount by 1.
        virtual Descriptor *get_buffer(int32_t global_id) const = 0;
        // The descriptor use a copy of the descriptor fd
        // The original descriptor could be used to recycle the
        // buffer when it is no more used by any process.
        // Buffer has refcount 1 after registering although
        // two NativeHandle are tracking it on client side.
        virtual Descriptor *register_buffer(size_t size, 
                                            NativeHandle fd,
                                            int32_t node_id,
                                            int32_t local_id) const = 0;
        // Acknowledge a notification received from the server
        // the notification should contain
        // node_id, local_id and global_id
        virtual void ack_notification() const = 0;
    };

    /*

    It is used directly in cg_pack.hpp  and
    in code transmitting a file descriptor on a socket.
    Any other use should go through the SharedBuffer API only
    SharedBuffer is the unique owner of a Descriptor in the application.
    */
    class Descriptor
    {
    public:
        virtual ~Descriptor() {};

        Descriptor() = delete;

        inline static Descriptor *(*mk_new_descriptor)(NativeHandle, size_t, int32_t) = nullptr;

        virtual void mapDescriptor() = 0;

        virtual NativeHandle fd() const noexcept = 0;

        // Check if descriptor is valid
        // but not if it is mapped
        virtual explicit operator bool() const noexcept = 0;

        size_t bytes() const noexcept
        {
            return size_;
        }

        void *data() noexcept
        {
            return data_;
        }

        const void *data() const noexcept
        {
            return data_;
        }

        void *operator->() { return data_; }

        // Use to lock access to corresponding mapped buffer
        // using the mem server
        int lock() const
        {
            if (MemServer::mem_server != nullptr)
            {
                return -1; // Error: IPCLock not used
            }
            if (global_id_ == -1)
            {
                fprintf(stderr, "Global id not set.\n");
                return -1; // Error: Lock offset not set
            }
            return MemServer::mem_server->lock(global_id_);
        }

        int read_lock() const
        {
            if (MemServer::mem_server != nullptr)
            {
                return -1; // Error: IPCLock not used
            }
            if (global_id_ == -1)
            {
                fprintf(stderr, "Global id not set.\n");
                return -1; // Error: Lock offset not set
            }
            return MemServer::mem_server->read_lock(global_id_);
        }

        int unlock() const
        {
            if (MemServer::mem_server != nullptr)
            {
                return -1; // Error: IPCLock not initialized or not used
            }
            if (global_id_ == -1)
            {
                fprintf(stderr, "Global id is not set.\n");
                return -1; // Error: Lock offset not set
            }
            return MemServer::mem_server->unlock(global_id_);
        }

        int read_unlock() const
        {
            if (MemServer::mem_server != nullptr)
            {
                return -1; // Error: IPCLock not initialized or not used
            }
            if (global_id_ == -1)
            {
                fprintf(stderr, "Global id is not set.\n");
                return -1; // Error: Lock offset not set
            }
            return MemServer::mem_server->read_unlock(global_id_);
        }

        int32_t refcount() const
        {
            if (MemServer::mem_server != nullptr)
            {
                return -1; // Error: IPCLock not initialized or not used
            }
            if (global_id_ == -1)
            {
                return -1; // Error: Lock offset not set
            }
            return MemServer::mem_server->refcount(global_id_);
        }

        int32_t global_id() const { return global_id_; }

    protected:
        Descriptor(size_t size, int32_t global_id) : data_(nullptr),
                                                     size_(size),
                                                     global_id_(global_id){};

        
        // When getting a shared lock, the API below returns a const
        // but we may still want to map the buffer to read it.
        mutable void *data_;
        size_t size_;
        int32_t global_id_;
    };

    // It should be the only owner of the Descriptor
    // So copy constructor are disabled
    template <typename T>
    class SharedBuffer
    {
    public:
        SharedBuffer(Descriptor *fd) : fd_(fd) {}

        SharedBuffer(const SharedBuffer &other) = delete;
        SharedBuffer(SharedBuffer &&other) noexcept
            : fd_(std::move(other.fd_))
        {
            other.fd_ = nullptr;
        }

        SharedBuffer &operator=(const SharedBuffer &other) = delete;

        SharedBuffer &operator=(SharedBuffer &&other) noexcept
        {
            if (this != &other)
            {
                fd_ = std::move(other.fd_);
                other.fd_ = nullptr;
            }
            return *this;
        }

        ~SharedBuffer()
        {
            delete fd_;
        }

        bool operator==(std::nullptr_t) const noexcept { return fd_->data() == nullptr; }
        bool operator!=(std::nullptr_t) const noexcept { return fd_->data() != nullptr; }

        T *get() noexcept { return static_cast<T *>(fd_->data()); }
        const T *get() const noexcept { return static_cast<const T *>(fd_->data()); }

        template <typename R>
        R *as() noexcept { return static_cast<R *>(fd_->data()); }

        template <typename R>
        const R *as() const noexcept { return static_cast<const R *>(fd_->data()); }

        T &operator[](std::size_t i) { return (static_cast<T *>(fd_->data()))[i]; }

        explicit operator bool() const noexcept { return fd_->data() != nullptr; }

        size_t bytes() const noexcept { return fd_->bytes(); }

        bool validFd() const noexcept
        {
            return ((fd_ != nullptr) && (*fd_));
        }

        const Descriptor *getDescriptor() const noexcept
        {
            return fd_;
        }

        Descriptor *getDescriptor() noexcept
        {
            return fd_;
        }

        int process_lock() const
        {
            int err = -1;
            if (fd_)
            {
                err = fd_->lock();
            }
            return (err);
        }

        int process_read_lock() const
        {
            int err = -1;
            if (fd_)
            {
                err = fd_->read_lock();
            }
            return (err);
        }

        int process_unlock() const
        {
            int err = -1;
            if (fd_)
            {
                err = fd_->unlock();
            }
            return (err);
        }

        int process_read_unlock() const
        {
            int err = -1;
            if (fd_)
            {
                err = fd_->read_unlock();
            }
            return (err);
        }

        void mapBuffer() const
        {
            fd_->mapDescriptor();
        }

        int32_t process_refcount() const
        {
            if (fd_)
            {
                return fd_->refcount();
            }
            return (-1);
        }

    protected:
        Descriptor *fd_;
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

        explicit UniquePtr(T *ptr,deleter_t d) noexcept
        {
            ptr_ = ptr;
            deleter_ = d;
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

        T *get() noexcept { return static_cast<T *>(ptr_); }
        const T *get() const noexcept { return static_cast<const T *>(ptr_); }

        template <typename R>
        R *as() noexcept { return static_cast<R *>(ptr_); }

        template <typename R>
        const R *as() const noexcept { return static_cast<const R *>(ptr_); }



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

    private:
        std::shared_ptr<T> obj;
        std::shared_ptr<CG_MUTEX> mutex;

    public:
        // Create with custom allocator and arguments for T
        template <typename... Args>
        static ProtectedBuffer create_with(Args &&...args)
        {
            auto mtx = std::allocate_shared<CG_MUTEX>(CG_MK_PROTECTED_MUTEX_ALLOCATOR(CG_MUTEX));
            // If mutex is nullptr then the object is also nullptr
            // But we can have a mutex and obj is nullptr
            if (mtx)
            {
                auto ptr = std::allocate_shared<T>(CG_MK_PROTECTED_BUF_ALLOCATOR(T), std::forward<Args>(args)...);
                return ProtectedBuffer(ptr, mtx);
            }
            else
            {
                return ProtectedBuffer{};
            }
        }

        // Exclusive (write) access
        template <typename Func>
        auto lock(Func &&f) -> decltype(f(CG_MUTEX_ERROR_TYPE(), false, std::declval<T &>()))
        {
            using R = decltype(f(CG_MUTEX_ERROR_TYPE(), false, std::declval<T &>()));
            if constexpr (std::is_void_v<R>)
            {
                if (mutex)
                {
                    CG_MUTEX_ERROR_TYPE error;
                    CG_ENTER_CRITICAL_SECTION(*mutex, error);
                    if (obj)
                    {
                        bool isShared = obj.use_count() > 1;
                        f(error, isShared, *obj);
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error);
                }

                return;
            }
            else
            {
                R r{};
                if (mutex)
                {
                    CG_MUTEX_ERROR_TYPE error;
                    CG_ENTER_CRITICAL_SECTION(*mutex, error);
                    if (obj)
                    {
                        bool isShared = obj.use_count() > 1;
                        r = f(error, isShared, *obj);
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error);
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
                if (mutex)
                {
                    CG_MUTEX_ERROR_TYPE error;
                    CG_ENTER_READ_CRITICAL_SECTION(*mutex, error);

                    if (obj)
                    {
                        f(error, *obj);
                    }
                    CG_EXIT_READ_CRITICAL_SECTION(*mutex, error);
                }

                return;
            }
            else
            {
                R r{};
                if (mutex)
                {
                    CG_MUTEX_ERROR_TYPE error;
                    CG_ENTER_READ_CRITICAL_SECTION(*mutex, error);

                    if (obj)
                    {
                        r = f(error, *obj);
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error);
                }

                return r;
            }
        }

        // Copyable and shareable
        ProtectedBuffer() = default;
        ProtectedBuffer(const ProtectedBuffer &other)
        {
            bool wasCopied = false;
            if (!other.mutex)
            {
                obj = nullptr;
                mutex = nullptr;
                return;
            }

            CG_MUTEX_ERROR_TYPE error;
            CG_ENTER_CRITICAL_SECTION(*other.mutex, error);
            if (!CG_MUTEX_HAS_ERROR(error))
            {
                wasCopied = true;
                obj = other.obj;
            }
            CG_EXIT_CRITICAL_SECTION(*other.mutex, error);

            if (!wasCopied)
            {
                mutex = nullptr;
                obj = nullptr;
            }
            else
            {
                mutex = other.mutex;
            }
        };

        ProtectedBuffer(ProtectedBuffer &&other) noexcept
        {
            bool wasMoved = false;
            if (!other.mutex)
            {
                obj = nullptr;
                mutex = nullptr;
                return;
            }

            CG_MUTEX_ERROR_TYPE error;
            CG_ENTER_CRITICAL_SECTION(*other.mutex, error);
            if (!CG_MUTEX_HAS_ERROR(error))
            {
                wasMoved = true;
                obj = std::move(other.obj);
            }
            CG_EXIT_CRITICAL_SECTION(*other.mutex, error);

            if (!wasMoved)
            {
                mutex = nullptr;
                obj = nullptr;
            }
            else
            {
                mutex = std::move(other.mutex);
            }
        };
        ProtectedBuffer &operator=(const ProtectedBuffer &other)
        {
            if (this != &other)
            {
                if ((mutex) && (other.mutex))
                {
                    bool wasCopied = false;
                    CG_MUTEX_ERROR_TYPE error_src;
                    CG_MUTEX_ERROR_TYPE error_dst;
                    CG_ENTER_CRITICAL_SECTION(*other.mutex, error_src);
                    CG_ENTER_CRITICAL_SECTION(*mutex, error_dst);

                    if (!CG_MUTEX_HAS_ERROR(error_src) && !CG_MUTEX_HAS_ERROR(error_dst))
                    {
                        // If no error, copy the object
                        wasCopied = true;
                        obj = other.obj;
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error_dst);
                    CG_EXIT_CRITICAL_SECTION(*other.mutex, error_src);
                    if (wasCopied)
                    {
                        mutex = other.mutex; // Share the mutex
                    }
                }
                else if ((mutex) && (!other.mutex))
                {
                    bool wasReset = false;
                    CG_MUTEX_ERROR_TYPE error_dst;
                    CG_ENTER_CRITICAL_SECTION(*mutex, error_dst);

                    if (!CG_MUTEX_HAS_ERROR(error_dst))
                    {
                        // If no error, reset the object
                        wasReset = true;
                        obj = nullptr;
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error_dst);
                    if (wasReset)
                    {
                        mutex = nullptr; // Reset the mutex if no error
                    }
                }
                else if ((!mutex) && (other.mutex))
                {
                    bool wasCopied = false;
                    CG_MUTEX_ERROR_TYPE error_src;
                    CG_ENTER_CRITICAL_SECTION(*other.mutex, error_src);

                    if (!CG_MUTEX_HAS_ERROR(error_src))
                    {
                        // If no error, copy the object
                        wasCopied = true;
                        obj = other.obj;
                    }

                    CG_EXIT_CRITICAL_SECTION(*other.mutex, error_src);
                    if (wasCopied)
                    {
                        mutex = other.mutex; // Share the mutex
                    }
                }
                else
                {
                    obj = nullptr;
                    mutex = nullptr;
                }
            }
            return *this;
        };
        ProtectedBuffer &operator=(ProtectedBuffer &&other) noexcept
        {
            if (this != &other)
            {
                if ((mutex) && (other.mutex))
                {
                    bool wasMoved = false;
                    CG_MUTEX_ERROR_TYPE error_src;
                    CG_MUTEX_ERROR_TYPE error_dst;
                    CG_ENTER_CRITICAL_SECTION(*other.mutex, error_src);
                    CG_ENTER_CRITICAL_SECTION(*mutex, error_dst);

                    if (!CG_MUTEX_HAS_ERROR(error_src) && !CG_MUTEX_HAS_ERROR(error_dst))
                    {
                        // If no error, copy the object
                        wasMoved = true;
                        obj = std::move(other.obj);
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error_dst);
                    CG_EXIT_CRITICAL_SECTION(*other.mutex, error_src);
                    if (wasMoved)
                    {
                        mutex = std::move(other.mutex); // Share the mutex
                    }
                }
                else if ((mutex) && (!other.mutex))
                {
                    bool wasReset = false;
                    CG_MUTEX_ERROR_TYPE error_dst;
                    CG_ENTER_CRITICAL_SECTION(*mutex, error_dst);

                    if (!CG_MUTEX_HAS_ERROR(error_dst))
                    {
                        // If no error, copy the object
                        wasReset = true;
                        obj = nullptr;
                    }

                    CG_EXIT_CRITICAL_SECTION(*mutex, error_dst);
                    if (wasReset)
                    {
                        mutex = nullptr; // Reset the mutex if no error
                    }
                }
                else if ((!mutex) && (other.mutex))
                {
                    bool wasMoved = false;
                    CG_MUTEX_ERROR_TYPE error_src;
                    CG_ENTER_CRITICAL_SECTION(*other.mutex, error_src);

                    if (!CG_MUTEX_HAS_ERROR(error_src))
                    {
                        // If no error, copy the object
                        wasMoved = true;
                        obj = std::move(other.obj);
                    }

                    CG_EXIT_CRITICAL_SECTION(*other.mutex, error_src);
                    if (wasMoved)
                    {
                        mutex = std::move(other.mutex); // Share the mutex
                    }
                }
                else
                {
                    obj = nullptr;
                    mutex = nullptr;
                }
            }
            return *this;
        };

    private:
        explicit ProtectedBuffer(std::shared_ptr<T> c, std::shared_ptr<CG_MUTEX> mtx) : obj(std::move(c)), mutex(std::move(mtx)) {}
    };

    /* Made to be used from a ProtectedBuffer and contained in a
    shared_ptr. The RawBuffer in itself must be unique. */
    struct RawBuffer
    {
        uint32_t buf_size;
        std::variant<UniquePtr<std::byte>, SharedBuffer<std::byte>> data;

        explicit RawBuffer(uint32_t size, UniquePtr<std::byte> &&buf_data) noexcept : buf_size(size)
        {
            if (buf_data == nullptr)
            {
                buf_size = 0; // Reset size if data is null
            }
            data = std::move(buf_data);
        }

        explicit RawBuffer(SharedBuffer<std::byte> &&buf_data) noexcept
        {
            // We don't test for nullptr since the
            // buffer may not be mapped
            // The descriptor inside sharedbuffer is tracking
            // the size of the buffer
            buf_size = (uint32_t)buf_data.bytes();

            data = std::move(buf_data);
        }

        RawBuffer(const RawBuffer &other) = delete;
        RawBuffer(RawBuffer &&other) noexcept : buf_size(other.buf_size), data(std::move(other.data))
        {
            other.buf_size = 0; // Reset the moved-from object
        }
        RawBuffer &operator=(const RawBuffer &other) = delete;

        RawBuffer &operator=(RawBuffer &&other) noexcept
        {
            if (this != &other)
            {
                buf_size = other.buf_size;
                data = std::move(other.data);
                other.buf_size = 0; // Reset the moved-from object
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
        std::variant<UniquePtr<T>, SharedBuffer<T>> data;

        explicit Tensor(uint8_t dimensions,
                        const cg_tensor_dims_t &dimensions_array,
                        UniquePtr<T> &&tensor_data) noexcept
            : nb_dims(dimensions), dims(dimensions_array)
        {
            if (tensor_data == nullptr)
            {
                nb_dims = 0; // Reset dimensions if data is null or size is zero
            }
            data = std::move(tensor_data);
        }

        explicit Tensor(uint8_t dimensions,
                        const cg_tensor_dims_t &dimensions_array,
                        SharedBuffer<T> &&tensor_data) noexcept
            : nb_dims(dimensions), dims(dimensions_array)
        {
            if (tensor_data == nullptr)
            {
                nb_dims = 0; // Reset dimensions if data is null or size is zero
            }

            data = std::move(tensor_data);
        }

        Tensor(const Tensor &other) = delete;

        Tensor(Tensor &&other) noexcept
            : nb_dims(other.nb_dims), dims(other.dims), data(std::move(other.data))
        {
            other.nb_dims = 0; // Reset the moved-from object
        }

        Tensor &operator=(const Tensor &other) = delete;
        Tensor &operator=(Tensor &&other) noexcept
        {
            if (this != &other)
            {
                nb_dims = other.nb_dims;
                dims = other.dims;
                data = std::move(other.data);
                other.nb_dims = 0; // Reset the moved-from object
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

        explicit operator bool() const noexcept { return !std::holds_alternative<std::monostate>(value); }

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

    // Shared_ptr is used to avoid increasing the side of the variant 
    // unique_ptr is not used because we do not want the type of the
    // deleter to be part of the variant type
    // Perhaps UniquePtr may be used ...
    using EventData = std::variant<cg_value, std::shared_ptr<ListValue>>;

    template <typename T>
    struct ValueParse
    {
        static inline bool contains(const cg_value &data) noexcept
        {
            return std::holds_alternative<T>(data.value);
        }

        static inline T getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<T>(data.value))
            {
                return std::get<T>(std::move(data.value));
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

        static inline TensorPtr<T> getValue(cg_value &&data) noexcept
        {
            if constexpr (std::is_const<T>::value)
            {

                if (std::holds_alternative<cg_any_const_tensor>(data.value))
                {
                    const cg_any_const_tensor &tensor = std::get<cg_any_const_tensor>(data.value);
                    if (std::holds_alternative<TensorPtr<T>>(tensor))
                    {
                        return std::get<TensorPtr<T>>(std::get<cg_any_const_tensor>(std::move(data.value)));
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
                        return std::get<TensorPtr<T>>(std::get<cg_any_tensor>(std::move(data.value)));
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

        static inline uint16_t getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<uint16_t>(data.value))
            {
                return std::get<uint16_t>(data.value);
            }
            else
            {
                return static_cast<uint16_t>(ValueParse<uint8_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline uint32_t getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<uint32_t>(data.value))
            {
                return std::get<uint32_t>(data.value);
            }
            else
            {
                return static_cast<uint32_t>(ValueParse<uint16_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline uint64_t getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<uint64_t>(data.value))
            {
                return std::get<uint64_t>(data.value);
            }
            else
            {
                return static_cast<uint64_t>(ValueParse<uint32_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline int16_t getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<int16_t>(data.value))
            {
                return std::get<int16_t>(data.value);
            }
            else
            {
                return static_cast<int16_t>(ValueParse<int8_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline int32_t getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<int32_t>(data.value))
            {
                return std::get<int32_t>(data.value);
            }
            else
            {
                return static_cast<int32_t>(ValueParse<int16_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline int64_t getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<int64_t>(data.value))
            {
                return std::get<int64_t>(data.value);
            }
            else
            {
                return static_cast<int64_t>(ValueParse<int32_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline float getValue(cg_value &&data) noexcept
        {
            if (std::holds_alternative<float>(data.value))
            {
                return std::get<float>(data.value);
            }
            else
            {
                return static_cast<float>(ValueParse<int32_t>::getValue(std::move(data))); // Fallback to uint32_t if not found
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

        static inline double getValue(cg_value &&data) noexcept
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
                return static_cast<double>(ValueParse<float>::getValue(std::move(data))); // Fallback to uint32_t if not found
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
        void apply_array_types(F &&f, O &&o, std::array<cg_value, CG_MAX_VALUES> &&values,
                               std::index_sequence<Is...>) const
        {
            (o.*f)(ValueParse<
                   typename std::tuple_element<Is, std::tuple<Args...>>::type>::getValue(std::move(values[Is]))...);
        };

    public:
        uint32_t event_id;
        uint32_t priority;
        EventData data;

        Event clone() const noexcept
        {
            Event evt;
            evt.event_id = this->event_id;
            evt.data = this->data;
            evt.priority = this->priority;
            return evt;
        }

        static std::shared_ptr<ListValue> make_new_list_value()
        {
            return std::allocate_shared<ListValue>(CG_MK_LIST_EVENT_ALLOCATOR(ListValue));
        }

        Event() noexcept : event_id(kNoEvent), priority(kNormalPriority)
        {
            data = cg_value();
        }

        explicit operator bool() const noexcept { return event_id != kNoEvent; }

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

        /*Event(const Event &other) noexcept
        {
            copyFrom(other);
        }*/

        Event(Event &&other) noexcept
        {
            moveFrom(std::move(other));
        }

        /*Event &operator=(const Event &other) noexcept
        {
            if (this != &other)
            {
                copyFrom(other);
            }
            return *this;
        }*/

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
        T get() noexcept
        {
            if (std::holds_alternative<cg_value>(data))
            {
                return ValueParse<T>::getValue(std::get<cg_value>(std::move(data)));
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
                    (o.*f)(ValueParse<Args...>::getValue(std::get<cg_value>(std::move(data)))); // Check if the single argument matches
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

                    std::shared_ptr<ListValue> cbv = std::get<std::shared_ptr<ListValue>>(std::move(data));
                    if (cbv && cbv->nb_values > 0)
                    {
                        if (sizeof...(Args) != cbv->nb_values)
                        {

                            return false; // Number of arguments does not match
                        }
                        else
                        {
                            apply_array_types<F, O, Args...>(std::forward<F>(f), std::forward<O>(o), std::move(cbv->values),
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

    /* Exchange messages with another process */
    class IPC
    {
    public:
        virtual ~IPC() {};

        virtual void send_message(int dstPortOrNodeId, Event &&evt) = 0;
        virtual Event receive_message(uint32_t &nodeId) = 0;

        inline static IPC *(*mk_new_ipc)(NativeHandle) = nullptr;
    };

    /* CMSIS Stream Node : event and streaming inherit from this class */
    class StreamNode
    {
    public:
        virtual ~StreamNode() {};

        StreamNode() {};

        virtual void processEvent(int dstPort, Event &&evt) {};
        virtual bool needsAsynchronousInit() const { return false; };
        virtual void subscribe(int outputPort, StreamNode &dst, int dstPort) {};
        virtual cg_status init() {return CG_SUCCESS;};

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
