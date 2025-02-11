// SPDX-License-Identifier: Apache License 2.0

#include <catch_amalgamated.hpp>
#include "datastructures/SecureBox.h"
#include "TestCommons.h"

using namespace tpunkt;


TEST_CASE("Assign Value")
{
    TEST_INIT();
    SecureBox<int> box;
    {
        auto reader = box.get();
        reader.get() = 5;
        REQUIRE(reader.get() == 5);
    }
    auto reader = box.get();
    REQUIRE(reader.get() == 5);
}
TEST_CASE("Move Box")
{
    TEST_INIT();
    SecureBox<int> box;
    {
        auto reader = box.get();
        reader.get() = 5;
        REQUIRE(reader.get() == 5);
    }
    {
        auto reader = box.get();
        REQUIRE(reader.get() == 5);
    }

    SecureBox<int> box2;
    box2 = std::move(box);

    auto reader = box2.get();
    REQUIRE(reader.get() == 5);


}