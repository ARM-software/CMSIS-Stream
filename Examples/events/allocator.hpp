#pragma once

#include <cstdlib>
#include <variant>

// Memory pool of combined values
struct StreamPool
{

    StreamPool(size_t block_size, int32_t size = 16)
        : block_size_(block_size),pool_size_(size), custom_buffer_(false)
    {
        CG_INIT_MUTEX(mutex);
        pool = new char[pool_size_ * block_size_];
        init_pool();
    }

    StreamPool(size_t block_size, char *buffer, int32_t size = 16)
        : block_size_(block_size),pool_size_(size), custom_buffer_(true)
    {
        CG_INIT_MUTEX(mutex);
        pool = buffer;
        init_pool();
    }

   
    ~StreamPool()
    {
        if (!custom_buffer_)
            delete[] pool; // Only delete if we did not use a custom buffer
        else
            pool = nullptr; // If we used a custom buffer, do not delete it
        
    }

    void free_value(void *cbv)
    {
        CG_MUTEX_ERROR_TYPE error;
        CG_ENTER_CRITICAL_SECTION(mutex,error);
        uint16_t block_index = (static_cast<char*>(cbv) - pool) / block_size_; // Calculate the index of the back entry
        if ((cbv != nullptr) && (block_index >= 0) && (block_index < pool_size_))
        {
            uint32_t *p = static_cast<uint32_t *>(static_cast<void *>(pool + block_index * block_size_));
            *p = current_free_; // Block point to current free
            current_free_ = block_index;       // Update the current free entry index
        }
        CG_EXIT_CRITICAL_SECTION(mutex,error);
    }

    void *new_value()
    {
        void *ret = nullptr;
        CG_MUTEX_ERROR_TYPE error;
        CG_ENTER_CRITICAL_SECTION(mutex,error);
        if (current_free_ >= 0)
        {
            uint32_t old_free = current_free_;                     // Get the current free entry index
            uint32_t *p = static_cast<uint32_t *>(static_cast<void *>(pool + current_free_ * block_size_));

            current_free_ = *p; // Update next_free to the next entry

            ret = reinterpret_cast<void *>(pool + old_free * block_size_); // Get the pointer to the new value

        }
        CG_EXIT_CRITICAL_SECTION(mutex,error);
        return ret;
    }


    size_t blockSize() const { return block_size_; }

protected:
   
    void init_pool()
    {
        current_free_ = 0;
        for (int32_t i = 0; i <= pool_size_ - 1; ++i)
        {
            uint32_t *p = static_cast<uint32_t *>(static_cast<void *>(pool + i * block_size_));

            if (i == pool_size_ - 1)
            {
                *p = -1;
            }
            else
            {
                *p = i + 1; // Initialize all entries to empty state
            }
        }
    }
    size_t block_size_;
    int32_t pool_size_;
    bool custom_buffer_;
    int32_t current_free_ = 0; // Index of the next free entry in the pool
    char *pool;
#if defined(CG_EVENTS) && defined(CG_EVENTS_MULTI_THREAD)
    CG_MUTEX mutex; // Mutex to protect access to the pool
#endif
};

#if defined(CG_EVENTS)
extern StreamPool *cg_cbv_pool;
#endif


template <typename T>
class StreamPoolAllocator
{
public:
    using value_type = T;

    StreamPoolAllocator() noexcept = default;

    template <typename U>
    StreamPoolAllocator(const StreamPoolAllocator<U> &) noexcept {}

    template <typename U>
    struct rebind
    {
        using other = StreamPoolAllocator<U>;
    };

    T *allocate(std::size_t n)
    {
        if ((n == 1) && (sizeof(T) <= cg_cbv_pool->blockSize()))
        {
            return static_cast<T *>(cg_cbv_pool->new_value());
        }
        else
        {
            return nullptr;
        }
    }

    void deallocate(T *p, std::size_t n) noexcept
    {
        cg_cbv_pool->free_value(static_cast<void*>(p));
    }


    // Optional but recommended: construct and destroy can be omitted
    // if you're using std::allocator_traits (it will use placement new)
};

template <typename T, typename U>
bool operator==(const StreamPoolAllocator<T> &, const StreamPoolAllocator<U> &) { return true; }

template <typename T, typename U>
bool operator!=(const StreamPoolAllocator<T> &, const StreamPoolAllocator<U> &) { return false; }
