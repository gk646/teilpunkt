// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_SPINLOCK_H
#define TPUNKT_SPINLOCK_H

#include <atomic>
#include "util/Macros.h"

namespace tpunkt
{
struct SpinlockGuard;

struct Spinlock
{
    Spinlock();

    [[nodiscard]] bool isLocked() const;

  private:
    void lock();
    void unlock();
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    bool hasGuard = false;
    TPUNKT_MACROS_MOVE_ONLY(Spinlock);
    friend SpinlockGuard;
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
    void lockReader();
    void unlockReader();
    void lockWriter();
    void unlockWriter();

    TPUNKT_MACROS_MOVE_ONLY(CooperativeSpinlock);
    std::atomic<int> readerCount{0};
    std::atomic<bool> writerFlag{false};
    std::atomic<bool> writerWaiting{false};
    friend struct CooperativeSpinlockGuard;
};

struct CooperativeSpinlockGuard final
{
    explicit CooperativeSpinlockGuard(CooperativeSpinlock& spinlock, const bool writeMode)
        : lock(spinlock), isWriteMode(writeMode)
    {
        if(writeMode)
        {
            lock.lockWriter();
        }
        else
        {
            lock.lockReader();
        }
    }

    ~CooperativeSpinlockGuard()
    {
        if(isWriteMode)
        {
            lock.unlockWriter();
        }
        else
        {
            lock.unlockReader();
        }
    }

  private:
    CooperativeSpinlock& lock;
    bool isWriteMode;
    TPUNKT_MACROS_STRUCT(CooperativeSpinlockGuard);
};

} // namespace tpunkt

#endif // TPUNKT_SPINLOCK_H