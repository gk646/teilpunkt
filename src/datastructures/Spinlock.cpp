// SPDX-License-Identifier: GPL-3.0-only

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
    while(exclusiveWaiting || exclusive)
    {
    }
    ++coopCount;
}

void CooperativeSpinlock::coopRemove()
{

}


void CooperativeSpinlock::exclusiveAdd()
{
}
void CooperativeSpinlock::exclusiveRemove()
{
}

} // namespace tpunkt