// SPDX-License-Identifier: GPL-3.0-only

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
        const auto totalSize = capacity * sizeof(BlockNode<T>);
        data = TPUNKT_MMAP(data, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
        for(uint32_t i = 0; i < capacity; ++i)
        {
            new(data + i) BlockNode<T>{i};
        }
    }

    ~BlockStorage()
    {
        const auto totalSize = capacity * sizeof(T);
        TPUNKT_MUNMAP(data, totalSize);
    }

    BlockNode<T>* operator[](const uint32_t idx)
    {
        return data[ idx ];
    }

    const BlockNode<T>* operator[](const uint32_t idx) const
    {
        return data[ idx ];
    }

    void alloc(uint32_t& idx)
    {
        if(freeList.empty())
        {
            if(isFull()) [[unlikely]]
            {
                LOG_FATAL("No space");
            }
            else if(isCloseToFull()) [[unlikely]]
            {
                LOG_WARNING("Block Storage almost full");
            }
            idx = size++;
        }
        else
        {
            idx = freeList.back();
            freeList.pop_back();
        }
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
    // TODO make queue and reuse front first
    std::vector<uint32_t> freeList;
    BlockNode<T>* data = nullptr;
    size_t size = 0;
    const size_t capacity = 0;
};

template <typename T>
struct BlockNode final
{
    explicit BlockNode(const uint32_t idx) : idx(idx)
    {
    }

    [[nodiscard]] uint32_t getNext() const
    {
        return next;
    }

    [[nodiscard]] bool hasNext() const
    {
        return next != NO_NEXT;
    }

    void setNext(const uint32_t nextId)
    {
        next = nextId;
    }

    [[nodiscard]] uint32_t getIdx() const
    {
        return idx;
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
    const uint32_t idx = 0;
    static constexpr uint32_t NO_NEXT = UINT32_MAX;
};


template <typename T>
struct BlockIterator final
{
    using NodeType = BlockNode<T>;
    using PointerType = std::conditional_t<std::is_const_v<T>, const NodeType*, NodeType*>;

    BlockIterator(PointerType start, const BlockStorage<BlockNode<T>>* storage) : ptr(start), storage(storage)
    {
    }

    static BlockIterator end()
    {
        return BlockIterator{nullptr, nullptr};
    }

    BlockNode<T>& operator*()
    {
        return *ptr;
    }

    const BlockNode<T>& operator*() const
    {
        return *ptr;
    }

    [[nodiscard]] uint32_t getIdx() const
    {
        return ptr->idx;
    }

    BlockIterator& operator++()
    {
        if(ptr->hasNext()) [[likely]]
        {
            ptr = &(*storage)[ ptr->getNext() ];
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
    PointerType ptr = nullptr;
    const BlockStorage<BlockNode<T>>* storage = nullptr;
};

template <typename T>
struct BlockList final
{
    BlockList() = default;

    BlockIterator<T> begin()
    {
        if(elems == 0) [[unlikely]]
        {
            return BlockIterator<T>{nullptr, nullptr};
        }
        auto& store = getStore();
        return BlockIterator<T>{store[ start ], store};
    }

    BlockIterator<T> end()
    {
        return BlockIterator<T>{nullptr, nullptr};
    }

    [[nodiscard]] uint32_t size() const
    {
        return elems;
    }

    void add(const T& val, uint32_t& idx)
    {
        auto& fileStore = getStore();

        uint32_t nextid = UINT32_MAX;
        fileStore.alloc(nextid);
        idx = nextid;

        auto* newBlock = fileStore[ nextid ];
        newBlock->get() = val;

        if(last == UINT32_MAX) [[unlikely]] // Empty
        {
            start = nextid;
            last = nextid;
        }
        else
        {
            auto* lastBlock = fileStore[ last ];
            lastBlock->setNext(nextid);
            last = nextid;
        }
        elems++;
    }

    bool remove(const uint32_t block)
    {
        if(elems == 0) [[unlikely]]
        {
            return false;
        }

        auto& fileStore = getStore();

        BlockNode<T>* prevPtr = nullptr;
        uint32_t prev = start;
        uint32_t it = start;
        while(it != UINT32_MAX)
        {
            auto* currentBlock = fileStore[ it ];
            if(it == block) [[unlikely]]
            {
                const uint32_t next = currentBlock->getNext();
                if(prevPtr == nullptr)                  // Deleted is first
                {
                    if(next == UINT32_MAX) [[unlikely]] // Only one
                    {
                        last = UINT32_MAX;
                        start = UINT32_MAX;
                    }
                    else                                // Multiple
                    {
                        start = next;
                    }
                }
                else                                    // Deleted is not the first
                {
                    if(next == UINT32_MAX)              // Deleted is last
                    {
                        last = prev;
                    }
                    else                                // Deleted is in between
                    {
                        prevPtr->setNext(next);
                    }
                }
                currentBlock->setNext(UINT32_MAX);
                fileStore.free(it);
                elems--;
                return true;
            }
            prevPtr = currentBlock;
            prev = it;
            it = currentBlock->getNext();
        }
        return false;
    }

    bool removeAll()
    {
        if(elems == 0) [[unlikely]]
        {
            return false;
        }

        auto& fileStore = getStore();
        uint32_t it = start;
        while(it != UINT32_MAX)
        {
            const auto current = it;
            auto* currentBlock = fileStore[ current ];
            it = currentBlock->getNext();
            currentBlock->setNext(UINT32_MAX);
            fileStore.free(current);
        }
        elems = 0;
        return true;
    }

    T* get(const uint32_t block)
    {
        if(elems == 0) [[unlikely]]
        {
            return nullptr;
        }

        auto& fileStore = getStore();
        uint32_t it = start;
        while(it != UINT32_MAX)
        {
            auto* currentBlock = fileStore[ it ];
            if(it == block) [[unlikely]]
            {
                return &fileStore[ it ]->get();
            }
            it = currentBlock->getNext();
        }

        return nullptr;
    }

    template <typename Func, typename... Args>
    void forEachBlock(Func func, Args&&... args)
    {
        if(elems == 0) [[unlikely]]
        {
            return;
        }
        auto& fileStore = getStore();
        uint32_t it = start;
        while(it != UINT32_MAX)
        {
            auto* currentBlock = fileStore[ it ];
            func(*currentBlock, std::forward<Args>(args)...);
            it = currentBlock->getNext();
        }
    }

  private:
    BlockStorage<BlockNode<T>>& getStore();

    uint32_t elems = 0;
    uint32_t start = UINT32_MAX;
    uint32_t last = UINT32_MAX;
};

} // namespace tpunkt

#endif // TPUNKT_BLOCKSTORAGE_H