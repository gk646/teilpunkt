// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_SPINLOCK_H
#define TPUNKT_SPINLOCK_H

#include <atomic>
#include "util/Macros.h"

namespace tpunkt
{

struct Spinlock
{
    Spinlock();
    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;
    Spinlock(Spinlock&&) noexcept;
    Spinlock& operator=(Spinlock&&) noexcept;

    [[nodiscard]] bool isLocked() const;

  private:
    void lock();
    void unlock();
    std::atomic<bool> flag;
    bool hasGuard = false;
    friend struct SpinlockGuard;
};

struct SpinlockGuard final
{
    explicit SpinlockGuard(Spinlock& spinlock);
    ~SpinlockGuard();

  private:
    Spinlock& lock;
    TPUNKT_MACROS_STRUCT(SpinlockGuard);
};

// Used for readers and writers
// Allows multiple readers but only 1 writer
// If a writer is waiting don't allow new readers
struct CooperativeSpinlock final
{
    [[nodiscard]] bool isLocked() const;

  private:
    void coopAdd();
    void coopRemove();
    void exclusiveAdd();
    void exclusiveRemove();

    std::atomic<int> coopCount{0};
    std::atomic<bool> exclusive{false};
    std::atomic<bool> exclusiveWaiting{false};
    friend struct CooperativeSpinlockGuard;
};

struct CooperativeSpinlockGuard final
{
    explicit CooperativeSpinlockGuard(CooperativeSpinlock& spinlock, const bool exclusive)
        : lock(&spinlock), exclusive(exclusive)
    {
        if(exclusive)
        {
            lock->exclusiveAdd();
        }
        else
        {
            lock->coopAdd();
        }
    }
    TPUNKT_MACROS_MOVE_ONLY(CooperativeSpinlockGuard);

    ~CooperativeSpinlockGuard()
    {
        if(exclusive)
        {
            lock->exclusiveRemove();
        }
        else
        {
            lock->coopRemove();
        }
    }

  private:
    CooperativeSpinlock* lock = nullptr; // Always valid
    bool exclusive = false;
};

} // namespace tpunkt

#endif // TPUNKT_SPINLOCK_H