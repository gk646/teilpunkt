// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_BUFFER_H
#define TPUNKT_BUFFER_H

#include <atomic>
#include <cstddef>
#include <cstdlib>
#include "util/Logging.h"
#include "util/Memory.h"

namespace tpunkt
{
struct Buffer final
{
    explicit Buffer(const size_t initialCapacity = 64) : cap(initialCapacity)
    {
        ptr = TPUNKT_ALLOC(ptr, cap);
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept : ptr(other.ptr), cap(other.cap)
    {
        other.ptr = nullptr;
        other.cap = 0;
    }

    Buffer& operator=(Buffer&& other) noexcept
    {
        if(this != &other)
        {
            ptr = other.ptr;
            cap = other.cap;
            other.ptr = nullptr;
            other.cap = 0;
        }
        return *this;
    }

    ~Buffer()
    {
        TPUNKT_FREE(ptr);
        ptr = nullptr;
    }

    void ensure(const size_t size)
    {
        expand(size);
    }

    [[nodiscard]] size_t capacity() const
    {
        return cap;
    }

    [[nodiscard]] unsigned char* data() const
    {
        return ptr;
    }

  private:
    unsigned char* ptr = nullptr;
    size_t cap = 0;

    void expand(const size_t newSize)
    {
        if(newSize >= cap)
        {
            cap = newSize;
            auto* newData = TPUNKT_REALLOC(ptr, cap);
            ptr = newData;
        }
    }
};


struct SyncedBuffer final
{
    explicit SyncedBuffer(const size_t baseCapacity = 64) : buffer(baseCapacity)
    {
    }
    SyncedBuffer(const SyncedBuffer&) = delete;
    SyncedBuffer& operator=(const SyncedBuffer&) = delete;

    // Tries to lock the buffer - returns true on success
    bool lock()
    {
        return !taken.exchange(true, std::memory_order_acquire);
    }

    void unlock()
    {
        taken.store(false, std::memory_order_release);
    }

    Buffer& getBuf()
    {
        return buffer;
    }

  private:
    std::atomic<bool> taken{false};
    Buffer buffer;
};


} // namespace tpunkt


#endif // TPUNKT_BUFFER_H