// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_BLOCKSTORAGE_H
#define TPUNKT_BLOCKSTORAGE_H

#include <cstdint>
#include <vector>
#include <sys/mman.h>
#include "util/Logging.h"
#include "util/Memory.h"

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
struct BlockStorage final
{
    explicit BlockStorage(const size_t capacity) : capacity(capacity)
    {
        const auto totalSize = capacity * sizeof(T);
        data = TPUNKT_MMAP(data, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
    }

    ~BlockStorage()
    {
        const auto totalSize = capacity * sizeof(T);
        TPUNKT_MUNMAP(data, totalSize);
    }

    T* operator[](const uint32_t idx)
    {
        return data[ idx ];
    }

    T* alloc(uint32_t& idx)
    {
        if(isFull())
        {
            LOG_FATAL("No space");
        }

        if(freeList.empty())
        {
            idx = size++;
        }
        else
        {
            idx = freeList.back();
            freeList.pop_back();
        }
        return data[ idx ];
    }

    void free(T* ptr)
    {
        free((uint32_t)(ptr - data));
    }

    void free(const uint32_t idx)
    {
        freeList.push_back(idx);
    }

    [[nodiscard]] bool isFull() const
    {
        return size >= capacity;
    }

    [[nodiscard]] bool isCloseToFull() const
    {
        return size >= (capacity * 0.9F);
    }

  private:
    std::vector<uint32_t> freeList;
    T* data = nullptr;
    size_t size = 0;
    const size_t capacity = 0;
};


template <typename T>
struct BlockNode final
{
    template <typename... Args>
    explicit BlockNode(Args args) : val(std::forward<Args>(args)...)
    {
    }

    uint32_t getNext() const
    {
        return next;
    }

    [[nodiscard]] bool hasNext() const
    {
        return next != NO_NEXT;
    }

    void setNext(const uint32_t idx)
    {
        if(hasNext())
        {
            LOG_ERROR("Node already has next");
        }
        else
        {
            next = idx;
        }
    }

    T& get()
    {
        return val;
    }

    const T& get() const
    {
        return val;
    }

  private:
    T val;
    uint32_t next = NO_NEXT;
    static constexpr uint32_t NO_NEXT = UINT32_MAX;
};

template <typename T>
struct BlockIterator final
{
    BlockIterator(BlockNode<T>* start, BlockStorage<BlockNode<T>>* storage) : ptr(start), storage(storage)
    {
    }

    T& operator*()
    {
        return ptr->get();
    }

    BlockIterator& operator++()
    {
        if(ptr->hasNext()) [[likely]]
        {
            ptr = storage[ ptr->getNext() ];
        }
        else
        {
            ptr = nullptr;
        }
        return *this;
    }

    BlockIterator operator++(int)
    {
        BlockIterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(const BlockIterator& other) const
    {
        return ptr == other.ptr;
    }

    bool operator!=(const BlockIterator& other) const
    {
        return ptr != other.ptr;
    }

  private:
    BlockNode<T>* ptr = nullptr;
    BlockStorage<BlockNode<T>>* storage = nullptr;
};

} // namespace tpunkt

#endif // TPUNKT_BLOCKSTORAGE_H