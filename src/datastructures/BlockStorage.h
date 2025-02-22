// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_BLOCKSTORAGE_H
#define TPUNKT_BLOCKSTORAGE_H

#include <cassert>
#include <cstdint>
#include <fcntl.h>
#include <vector>
#include <sys/mman.h>
#include "datastructures/Iterator.h"
#include "util/Logging.h"
#include "util/Memory.h"

namespace tpunkt
{

// This should be the best approach for laying out memory for the file system
// Properties:
//      - Pointer Stability (vital for caching - can cache pointer to blocks)
//      - Free list / Filling up holes if blocks free up
//      - Avoids dynamic structure per file
//      - No fragmentation
//      - No indirection level on access
//
// Cons:
//      - Potentially need to iterate and jump memory -> but should be cached or close together (continuous)
//      - Total size needs to be known upfront (otherwise always needs additional indirection ? - but at runtime)

using BlockIndex = uint32_t;

template <typename T, size_t blockSize>
struct StaticBlock final
{
    using BlockStore = BlockStorage<T, blockSize>;

    [[nodiscard]] bool hasNext() const
    {
        return next != NO_NEXT;
    }

    [[nodiscard]] bool isFull() const
    {
        return size == blockSize;
    }

    Iterator<T> begin()
    {
        return {arr};
    }

    Iterator<T> end()
    {
        return {arr + size};
    }

    void add(BlockStore* store, const T& value);
    void add(BlockStore* store, T&& value);

    // If returns true stops
    using IterateFunc = bool (*)(T&);
    void iterate(BlockStore* store, IterateFunc func);

    // If returns true deletes the current element
    template <typename CompareFunc>
    bool remove(BlockStore* store, CompareFunc compare);

  private:
    static constexpr uint32_t NO_NEXT = UINT32_MAX;
    T arr[ blockSize ];
    uint32_t next = NO_NEXT;
    uint8_t size = 0;
    friend BlockStorage;
};

template <typename T, size_t blockSize>
struct BlockStorage final
{
    using StaticBlock = StaticBlock<T, blockSize>;

    explicit BlockStorage(const size_t blocks) : blocks(blocks)
    {
        const auto totalSize = blocks * sizeof(T);
        blockData = TPUNKT_MMAP(blockData, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
    }

    ~BlockStorage()
    {
        const auto totalSize = blocks * sizeof(T);
        TPUNKT_MUNMAP(blockData, totalSize);
    }

    BlockIndex blockGetNew()
    {
        if(isFull())
        {
            LOG_FATAL("No space");
        }
        return blockData[ size++ ];
    }

    void blockAdd(StaticBlock& block, T&& value)
    {
        auto* last = &getLast(block);
        if(last->isFull()) [[unlikely]]
        {
            assert(last->hasNext());
            last->next = blockGetNew();
            last = blockData[ last->next ];
        }
        last->arr[ size++ ] = std::move(value);
    }

    void blockAdd(BlockIndex index, const T& value)
    {
        auto& block = blockData[ index ];
        auto* last = &getLast(block);
        if(last->isFull()) [[unlikely]]
        {
            assert(last->hasNext());
            last->next = blockGetNew();
            last = blockData[ last->next ];
        }
        last->arr[ size++ ] = value;
    }

    bool blockRemove(StaticBlock& index, const T& value)
    {
    }

    [[nodiscard]] bool isFull() const
    {
        return size >= blocks;
    }

    [[nodiscard]] bool isCloseToFull() const
    {
        return size >= (blocks * 0.9F);
    }

  private:
    StaticBlock& getLast(StaticBlock& begin)
    {
        StaticBlock* start = &begin;
        while(start->hasNext())
        {
            start = blockData[ start->next ];
        }
        return *start;
    }

    StaticBlock* blockData;
    size_t size = 0;
    size_t blocks = 0;
};


template <typename T, size_t blockSize>
void StaticBlock<T, blockSize>::add(BlockStore* store, const T& value)
{
    store->blockAdd(*this, value);
}

template <typename T, size_t blockSize>
void StaticBlock<T, blockSize>::add(BlockStore* store, T&& value)
{
    store->blockAdd(*this, std::move(value));
}

template <typename T, size_t blockSize>
void StaticBlock<T, blockSize>::iterate(BlockStore* store, const IterateFunc func)
{
    StaticBlock* start = &this;
    while(true)
    {
        for(uint8_t i = 0; i < start->size; ++i)
        {
            if(!func(start->arr[ i ]))
            {
                return;
            }
        }

        if(start->hasNext())
        {
            start = store->blockData[ start->next ];
        }
        else
        {
            return;
        }
    }
}

template <typename T, size_t blockSize>
template <typename CompareFunc>
bool StaticBlock<T, blockSize>::remove(BlockStore* store, CompareFunc compare)
{
    StaticBlock* start = &this;
    bool removed = false;
    while(true)
    {
        for(uint8_t i = 0; i < start->size; ++i)
        {
            if(!func(start->arr[ i ]))
            {
                start->arr[ i ] = std::move(start->arr[ start->size - 1 ]);
                --start->size;
            }
        }

        if(start->hasNext())
        {
            start = store->blockData[ start->next ];
        }
        else
        {
            break;
        }
    }
}

} // namespace tpunkt

#endif // TPUNKT_BLOCKSTORAGE_H