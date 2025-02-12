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
    TPUNKT_MACROS_MOVE_ONLY(Spinlock);

    [[nodiscard]] bool isLocked() const;

  private:
    void lock();
    void unlock();
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    bool hasGuard = false;
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


} // namespace tpunkt

#endif // TPUNKT_SPINLOCK_H