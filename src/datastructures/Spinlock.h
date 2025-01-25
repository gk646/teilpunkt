#ifndef TPUNKT_SPINLOCK_H
#define TPUNKT_SPINLOCK_H

#include <atomic>

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
        friend SpinlockGuard;
    };


    struct SpinlockGuard final
    {
        explicit SpinlockGuard(Spinlock& spinlock);
        ~SpinlockGuard();

      private:
        Spinlock& lock;
    };


} // namespace tpunkt

#endif // TPUNKT_SPINLOCK_H