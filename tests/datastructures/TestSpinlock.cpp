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