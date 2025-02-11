// SPDX-License-Identifier: Apache License 2.0

#include <catch_amalgamated.hpp>
#include "datastructures/SecureList.h"
#include "TestCommons.h"

using namespace tpunkt;

TEST_CASE("SecureList Basic Functionality")
{
    TEST_INIT();
    SECTION("Default Construction")
    {
        SecureList<int> list;
        auto access = list.get();

        REQUIRE(access.begin() == access.end());
        REQUIRE(access.contains(0) == false);
    }

    SECTION("Construction with Initial Capacity")
    {
        SecureList<int> list(5);
        auto access = list.get();

        REQUIRE(access.begin() == access.end());
        REQUIRE(access.contains(0) == false);
    }

    SECTION("Push Back and Size Check")
    {
        SecureList<int> list(5);
        auto access = list.get();

        access.push_back(42);
        access.push_back(7);

        REQUIRE(*access.begin() == 42);
        REQUIRE(access.size() == 2);
        REQUIRE(access.contains(42) == true);
        REQUIRE(access.contains(7) == true);
        REQUIRE(access.findIndex(42) == 0);
        REQUIRE(access.findIndex(7) == 1);
    }

    SECTION("Push Beyond Initial Capacity")
    {
        SecureList<int> list(2);
        auto access = list.get();

        access.push_back(1);
        access.push_back(2);
        access.push_back(3); // Should trigger growth

        REQUIRE(access.contains(3) == true);
        REQUIRE(access.findIndex(3) == 2);
    }

    SECTION("Erase Element")
    {
        SecureList<int> list(5);
        auto access = list.get();

        access.push_back(1);
        access.push_back(2);
        access.push_back(3);

        REQUIRE(access.erase(2) == true);
        REQUIRE(access.contains(2) == false);
        REQUIRE(access.findIndex(3) == 1); // Order not preserved
    }

    SECTION("Emplace Back Element")
    {
        SecureList<int> list(5);
        auto access = list.get();

        auto& ref = access.emplace_back();
        ref = 99;

        REQUIRE(access.contains(99) == true);
        REQUIRE(*access.begin() == 99);
    }

    SECTION("Iterator Functionality")
    {
        SecureList<int> list(5);
        auto access = list.get();

        access.push_back(10);
        access.push_back(20);

        std::vector<int> values;
        for(auto& value : access)
        {
            values.push_back(value);
        }

        REQUIRE(values.size() == 2);
        REQUIRE(values[ 0 ] == 10);
        REQUIRE(values[ 1 ] == 20);
    }

    SECTION("Out of Bounds Access")
    {
        SecureList<int> list(5);
        auto access = list.get();

        access.push_back(1);
        access.push_back(2);
    }

    SECTION("Move Constructor and Assignment")
    {
        SecureList<int> list1(5);
        {
            auto access1 = list1.get();
            access1.push_back(123);
        }

        SecureList<int> list2 = std::move(list1); // Move constructor
        {
            auto access2 = list2.get();
            access2.push_back(1);
            REQUIRE(access2.contains(123) == true);
        }

        SecureList<int> list3;
        {
            list3 = std::move(list2);             // Move assignment
            auto access3 = list3.get();
            REQUIRE(access3.contains(123) == true);
            REQUIRE(access3.size() == 2);
        }
    }

    SECTION("Erasing")
    {
        SecureList<int> list(10);
        auto access = list.get();
        constexpr int limit = 11;
        for(int i = 0; i < limit; ++i)
        {
            access.push_back(i);
        }

        REQUIRE(access.size() == limit);

        for(int i = 0; i < limit; ++i)
        {
            access.erase(i);
        }

        REQUIRE(access.size() == 0);

        for(int i = 0; i < limit; ++i)
        {
            access.push_back(i);
        }

        for(int i = 0; i < limit; ++i)
        {
            access.eraseIndex(limit - (i + 1));
        }
        REQUIRE(access.size() == 0);
    }
}

TEST_CASE("SecureList Initializer Check")
{
    TEST_INIT();
    struct TestStruct
    {
        int x;
        TestStruct() : x(0)
        {
        } // Default constructor
        TestStruct(int value) : x(value)
        {
        }
        bool operator==(const TestStruct& other) const
        {
            return x == other.x;
        }
    };

    SECTION("Initialization and Construction")
    {
        SecureList<TestStruct> list(5);
        auto access = list.get();

        access.emplace_back().x = 42;
        access.push_back(TestStruct(7));

        REQUIRE(access[ 0 ].x == 42);
        REQUIRE(access[ 1 ].x == 7);
    }
}