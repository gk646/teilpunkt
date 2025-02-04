#include <catch_amalgamated.hpp>
#include "TestCommons.h"
#include "storage/datastore/StorageTransaction.h"

using namespace tpunkt;

TEST_CASE("Simple Storage Transaction")
{
    TEST_INIT();
    bool variable = false;
    auto exitFunc = [ & ]() { variable = true; };
    {
        StorageTransaction t{};
    }
}