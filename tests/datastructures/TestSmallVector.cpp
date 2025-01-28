#include <catch_amalgamated.hpp>
#include "datastructures/SmallVector.h"
#include "TestCommons.h"

using namespace tpunkt;


TEST_CASE("Adding elements")
{
    TEST_INIT();
    constexpr int LIMIT = 10;
    int buf[ LIMIT ]{};
    Collector<int> col{buf, 10 * sizeof(int)};


    while(!col.push_back(5))
    {
    }

    REQUIRE(col.size() == LIMIT);
    REQUIRE_FALSE(col.hasAll());

    col.setHasAll();
    REQUIRE(col.hasAll());
}