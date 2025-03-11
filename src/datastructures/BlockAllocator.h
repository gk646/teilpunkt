#ifndef TPUNKT_BLOCK_ALLOCATOR
#define TPUNKT_BLOCK_ALLOCATOR

#include <sys/mman.h>
#include "datastructures/Spinlock.h"

namespace tpunkt
{

// This should be the best approach for laying out memory for the file system
// Properties:
//      - Pointer Stability (vital for caching - can cache pointer to blocks)
//      - Free list / Filling up holes if blocks free up
//      - Avoids dynamic structure in each file/directory (fragmentation, cache locality)
//      - No fragmentation
//      - Only one indirection level on access
//
// Cons:
//      - Potentially need to iterate and jump memory -> but should be cached or close together (continuous)
//      - Total size needs to be known upfront (otherwise always needs additional indirection ? - but at runtime)
//        We cant realloc() cause used by many threads at any time

// One could say that through the nature of increasing size for later allocations: higher size == later created
// This would mean that some information about a file leaks by knowing the id (side channel) - but through the freelist
// you never know if it was freed and reused or is still the original id (in theory)

template <typename T>
struct BlockAllocator final
{
    explicit BlockAllocator(const size_t capacity) : capacity(capacity)
    {
        const auto totalSize = capacity * sizeof(T);
        data = TPUNKT_MMAP(data, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
    }

    ~BlockAllocator()
    {
        const auto totalSize = capacity * sizeof(T);
        TPUNKT_MUNMAP(data, totalSize);
    }

    T* allocate(const size_t n)
    {
        SpinlockGuard guard{lock};
        if(n > 1)
        {
            LOG_FATAL("Invalid size");
        }

        if(freeIndex == nullptr)
        {
            if(size == capacity) [[unlikely]]
            {
                LOG_FATAL("No space");
            }
            else if(size >= static_cast<size_t>(static_cast<float>(capacity) * 0.9F)) [[unlikely]]
            {
                LOG_WARNING("Block Storage almost full");
            }
            return &data[ size++ ];
        }

        const auto lastFree = (T*)freeIndex;
        if(*freeIndex == UINT32_MAX)
        {
            freeIndex = nullptr;
        }
        else
        {
            freeIndex = (size_t*)&data[ *freeIndex ];
        }
        return lastFree;
    }

    void deallocate(T* ptr, size_t /**/)
    {
        SpinlockGuard guard{lock};
        if(freeIndex == nullptr)
        {
            freeIndex = (size_t*)ptr;
            *freeIndex = UINT32_MAX;
        }
        else
        {
            const auto lastFreeIndex = ptr - data;
            freeIndex = (size_t*)ptr;
            *freeIndex = lastFreeIndex;
        }
    }

  private:
    T* data = nullptr;
    size_t* freeIndex = nullptr;
    size_t capacity = 0;
    size_t size = 0;
    Spinlock lock;
};


template <typename T>
struct SharedBlockAllocator
{
    using value_type = T;

    SharedBlockAllocator()
    {
        if(alloc == nullptr)
        {
            alloc = new BlockAllocator<T>(1000);
        }
    }
    template <typename U>
    explicit SharedBlockAllocator(const SharedBlockAllocator<U>& other)
    {

    }


    value_type* allocate(size_t n)
    {
        return alloc->allocate(n);
    }

    void deallocate(T* ptr, size_t n)
    {
        alloc->deallocate(ptr, n);
    }

  private:
    static BlockAllocator<T>* alloc;
};

template <typename T>
BlockAllocator<T>* SharedBlockAllocator<T>::alloc = nullptr;

} // namespace tpunkt

#endif // TPUNKT_BLOCK_ALLOCATOR