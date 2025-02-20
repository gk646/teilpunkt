// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_BLOCKSTORAGE_H
#define TPUNKT_BLOCKSTORAGE_H

#include <assert.h>
#include <cstdint>
#include <fcntl.h>
#include <vector>
#include <sys/mman.h>
#include "datastructures/Iterator.h"
#include "util/Logging.h"
#include "util/Memory.h"

namespace tpunkt
{

using BlockIndex = uint32_t;

template <typename T, size_t blockSize>
struct StaticBlock final
{

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

    void blockAdd(BlockIndex index, T&& value)
    {
        auto& block = blockData[ index ];
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

    bool blockRemove(const T& value)
    {
    }

    [[nodiscard]] bool isFull() const
    {
        return size >= blocks;
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


} // namespace tpunkt

#endif // TPUNKT_BLOCKSTORAGE_H