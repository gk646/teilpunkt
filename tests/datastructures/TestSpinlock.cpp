// SPDX-License-Identifier: GPL-3.0-only

#include <catch_amalgamated.hpp>
#include <thread>
#include "datastructures/Spinlock.h"
#include "TestCommons.h"

using namespace tpunkt;

TEST_CASE("Test Locking")
{
    TEST_INIT();
    Spinlock spinlock;

    int num = 0;
    constexpr int limit = 100000;

    volatile bool done1 = false;
    volatile bool done2 = false;
    auto inc = [ & ]()
    {
        for(int i = 0; i < limit; ++i)
        {
            SpinlockGuard guard{spinlock};
            num++;
        }
        done1 = true;
    };

    auto inc2 = [ & ]()
    {
        for(int i = 0; i < limit; ++i)
        {
            SpinlockGuard guard{spinlock};
            num++;
        }
        done2 = true;
    };

    std::thread runner1{inc};
    std::thread runner2{inc2};

    runner1.join();
    runner2.join();

    while(!done1 || !done2)
    {
    }
    REQUIRE(num == limit * 2);
}

TEST_CASE("Multiple cooperative locks are allowed concurrently")
{
    CooperativeSpinlock lock;
    const int numReaders = 10;
    std::atomic<int> concurrentReaders{0};
    std::atomic<int> maxConcurrentReaders{0};
    std::vector<std::thread> threads;

    for(int i = 0; i < numReaders; ++i)
    {
        threads.emplace_back(
            [ & ]()
            {
                {
                    // Acquire cooperative (reader) lock.
                    CooperativeSpinlockGuard guard(lock, false);
                    int current = ++concurrentReaders;
                    // Update maximum concurrent readers seen.
                    int oldMax = maxConcurrentReaders.load();
                    while(current > oldMax && !maxConcurrentReaders.compare_exchange_weak(oldMax, current))
                    {
                    }
                    // Hold the lock briefly.
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    concurrentReaders.fetch_sub(1);
                }
            });
    }
    for(auto& t : threads)
    {
        t.join();
    }
    // Expect that all readers could hold the lock concurrently.
    REQUIRE(maxConcurrentReaders.load() == numReaders);
}

TEST_CASE("Only one exclusive lock is held at a time")
{
    CooperativeSpinlock lock;
    std::atomic<int> writerActive{0};
    std::atomic<int> maxWriterActive{0};
    const int numWriters = 5;
    std::vector<std::thread> threads;

    auto writerTask = [ & ]()
    {
        {
            // Acquire exclusive (writer) lock.
            CooperativeSpinlockGuard guard(lock, true);
            int curr = ++writerActive;
            int oldMax = maxWriterActive.load();
            while(curr > oldMax && !maxWriterActive.compare_exchange_weak(oldMax, curr))
            {
            }
            // Hold the exclusive lock for a short duration.
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            writerActive.fetch_sub(1);
        }
    };

    for(int i = 0; i < numWriters; ++i)
    {
        threads.emplace_back(writerTask);
    }
    for(auto& t : threads)
    {
        t.join();
    }

    REQUIRE(maxWriterActive.load() == 1);
}

TEST_CASE("New readers are blocked when a writer is active")
{
    CooperativeSpinlock lock;
    std::atomic<bool> readerAcquired{false};
    std::chrono::steady_clock::time_point readerStart;
    std::chrono::steady_clock::time_point readerEnd;

    std::thread writer(
        [ & ]()
        {
            CooperativeSpinlockGuard guard(lock, true);
            // Hold the exclusive lock for 200ms.
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        });

    // Allow writer to start.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Reader task: attempt to acquire a cooperative lock.
    std::thread reader(
        [ & ]()
        {
            readerStart = std::chrono::steady_clock::now();
            CooperativeSpinlockGuard guard(lock, false);
            readerEnd = std::chrono::steady_clock::now();
            readerAcquired.store(true);
        });

    writer.join();
    reader.join();

    // The reader should have waited for the writer.
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(readerEnd - readerStart).count();
    // Allow a little tolerance for scheduling delays.
    REQUIRE(elapsed >= 149);
    REQUIRE(readerAcquired.load() == true);
}