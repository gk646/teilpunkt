// SPDX-License-Identifier: GPL-3.0-only

#include <catch_amalgamated.hpp>
#include "TestCommons.h"
#include "storage/StorageTransaction.h"

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