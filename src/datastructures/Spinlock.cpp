// SPDX-License-Identifier: Apache License 2.0

#include <sched.h>
#include "datastructures/Spinlock.h"
#include "util/Logging.h"

namespace tpunkt
{
    Spinlock::Spinlock()
    {
        flag.clear(std::memory_order_release);
    }

    bool Spinlock::isLocked() const
    {
        return flag.test(std::memory_order_acquire);
    }

    void Spinlock::lock()
    {
        while(flag.test_and_set(std::memory_order_acquire))
        {
            sched_yield();
        }
    }

    void Spinlock::unlock()
    {
        flag.clear(std::memory_order_release);
    }

    SpinlockGuard::SpinlockGuard(Spinlock& spinlock) : lock(spinlock)
    {

        lock.lock();
        lock.hasGuard = true;
    }

    SpinlockGuard::~SpinlockGuard()
    {
        lock.hasGuard = false;
        lock.unlock();
    }
} // namespace tpunkt