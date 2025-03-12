// SPDX-License-Identifier: GPL-3.0-only

#include <mutex>
#include "datastructures/Spinlock.h"

namespace tpunkt
{

Spinlock& Spinlock::operator=(Spinlock&& other) noexcept
{
    if(this != &other)
    {
        flag.store(other.flag.load(std::memory_order_relaxed), std::memory_order_relaxed);
        other.flag.store(false, std::memory_order_relaxed);
    }
    return *this;
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
}

SpinlockGuard::~SpinlockGuard()
{
    lock.unlock();
}

void CooperativeSpinlock::coopAdd()
{
    while(exclusiveCount.load(std::memory_order_acquire) > 0)
    {
        sched_yield();
    }
    {
        SpinlockGuard guard{unique};
        coopCount.fetch_add(1, std::memory_order_acquire);
    }
}

void CooperativeSpinlock::coopRemove()
{
    coopCount.fetch_sub(1, std::memory_order_release);
}

void CooperativeSpinlock::exclusiveAdd()
{
    {
        SpinlockGuard guard{unique};
        exclusiveCount.fetch_add(1, std::memory_order_release);
    }

    while (coopCount.load(std::memory_order_acquire) > 0) {
        sched_yield();
    }
    exclusive.lock();
}

void CooperativeSpinlock::exclusiveRemove()
{
    exclusive.unlock();
    exclusiveCount.fetch_sub(1, std::memory_order_release);
}

} // namespace tpunkt