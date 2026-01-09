#pragma once 

#include <zephyr/kernel.h>
#include <cstdio>
#include <type_traits>

extern "C"
{
extern struct k_mem_slab cg_eventPool;
extern struct k_mem_slab cg_bufPool;
extern struct k_mem_slab cg_mutexPool;
}

template <typename T>
class ZephyrEventPoolAllocator
{
public:
    using value_type = T;
    using is_always_equal = std::true_type;

    ZephyrEventPoolAllocator() noexcept = default;

    template <typename U>
    ZephyrEventPoolAllocator(const ZephyrEventPoolAllocator<U> &) noexcept {}

    template <typename U>
    struct rebind
    {
        using other = ZephyrEventPoolAllocator<U>;
    };

    T *allocate(std::size_t n)
    {
        if ((n == 1) && (sizeof(T) <= cg_eventPool.info.block_size))
        {
            T* p;
            k_mem_slab_alloc(&cg_eventPool, (void **)&p, K_FOREVER);
            return p;
        }
        else
        {
           //throw std::bad_alloc(); // required by standard
           return nullptr;
        }
    };

    void deallocate(T *p, std::size_t n) noexcept
    {
        k_mem_slab_free(&cg_eventPool,static_cast<void*>(p));
    };


    // Optional but recommended: construct and destroy can be omitted
    // if you're using std::allocator_traits (it will use placement new)
};

template <typename T, typename U>
bool operator==(const ZephyrEventPoolAllocator<T> &, const ZephyrEventPoolAllocator<U> &) { return true; }

template <typename T, typename U>
bool operator!=(const ZephyrEventPoolAllocator<T> &, const ZephyrEventPoolAllocator<U> &) { return false; }

template <typename T>
class ZephyrBufPoolAllocator
{
public:
    using value_type = T;
    using is_always_equal = std::true_type;

    ZephyrBufPoolAllocator() noexcept = default;

    template <typename U>
    ZephyrBufPoolAllocator(const ZephyrBufPoolAllocator<U> &) noexcept {}

    template <typename U>
    struct rebind
    {
        using other = ZephyrBufPoolAllocator<U>;
    };

    T *allocate(std::size_t n)
    {
        if ((n == 1) && (sizeof(T) <= cg_bufPool.info.block_size))
        {
            T* p;
            k_mem_slab_alloc(&cg_bufPool, (void **)&p, K_FOREVER);
            return p;
        }
        else
        {
            //throw std::bad_alloc();
            return nullptr;
        }
    };

    void deallocate(T *p, std::size_t n) noexcept
    {
        k_mem_slab_free(&cg_bufPool,static_cast<void*>(p));
    };


    // Optional but recommended: construct and destroy can be omitted
    // if you're using std::allocator_traits (it will use placement new)
};

template <typename T, typename U>
bool operator==(const ZephyrBufPoolAllocator<T> &, const ZephyrBufPoolAllocator<U> &) { return true; }

template <typename T, typename U>
bool operator!=(const ZephyrBufPoolAllocator<T> &, const ZephyrBufPoolAllocator<U> &) { return false; }

template <typename T>
class ZephyrMutexPoolAllocator
{
public:
    using value_type = T;
    using is_always_equal = std::true_type;

    ZephyrMutexPoolAllocator() noexcept = default;

    template <typename U>
    ZephyrMutexPoolAllocator(const ZephyrMutexPoolAllocator<U> &) noexcept {}

    template <typename U>
    struct rebind
    {
        using other = ZephyrMutexPoolAllocator<U>;
    };

    T *allocate(std::size_t n)
    {
        if ((n == 1) && (sizeof(T) <= cg_mutexPool.info.block_size))
        {
           T* p;
           k_mem_slab_alloc(&cg_mutexPool, (void **)&p, K_FOREVER);
           return p;
        }
        else
        {
            //throw std::bad_alloc();
            return nullptr;
        }
    };

    void deallocate(T *p, std::size_t n) noexcept
    {
        k_mem_slab_free(&cg_mutexPool,static_cast<void*>(p));
    };


    // Optional but recommended: construct and destroy can be omitted
    // if you're using std::allocator_traits (it will use placement new)
};

template <typename T, typename U>
bool operator==(const ZephyrMutexPoolAllocator<T> &, const ZephyrMutexPoolAllocator<U> &) { return true; }

template <typename T, typename U>
bool operator!=(const ZephyrMutexPoolAllocator<T> &, const ZephyrMutexPoolAllocator<U> &) { return false; }
