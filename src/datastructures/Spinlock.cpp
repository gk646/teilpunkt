// SPDX-License-Identifier: Apache License 2.0


#include "datastructures/Spinlock.h"

namespace tpunkt
{
Spinlock::Spinlock() : flag(false)
{
}

Spinlock::Spinlock(Spinlock&& other) noexcept
{
}

Spinlock& Spinlock::operator=(Spinlock&& other ) noexcept
{
}

bool Spinlock::isLocked() const
{
    return flag.load(std::memory_order_acquire);
}

void Spinlock::lock()
{
    while(flag.exchange(true, std::memory_order_acquire))
    {
        sched_yield();
    }
}

void Spinlock::unlock()
{
    flag.store(false, std::memory_order_release);
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