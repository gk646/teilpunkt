// SPDX-License-Identifier: GPL-3.0-only

#include <catch_amalgamated.hpp>
#include "storage/vfs/VirtualDirectory.h"
#include "TestCommons.h"

using namespace tpunkt;

// Mocking or creating mock data

UserID getUserID()
{
    return UserID{54321};
}

FileCreationInfo getFileInfo(const FileName& name)
{
    return {.name = name, .creator = getUserID()};
}

DirectoryCreationInfo getDirInfo(const char* name, uint64_t maxSize, VirtualDirectory* parent)
{
    return {.name = FileName{name}, .maxSize = maxSize, .parent = parent, .creator = getUserID()};
}

TEST_CASE("Virtual Directory")
{
    TEST_INIT();

    constexpr auto sizeLimit = 1000;
    auto baseDir = VirtualDirectory{getDirInfo("Base Dir", sizeLimit, nullptr)};

    {
        REQUIRE(baseDir.getStats().accessCount == 0);
        REQUIRE(baseDir.getStats().changeCount == 0);

        REQUIRE(baseDir.getStats().lastAccess.isInPast());
        REQUIRE(baseDir.getStats().lastEdit.isInPast());
        REQUIRE(baseDir.getStats().creation.isInPast());

        REQUIRE(baseDir.getLimits().sizeLimit == sizeLimit);
    }

    SECTION("Add File")
    {
        baseDir.fileAdd(getFileInfo("File1"));
        REQUIRE(baseDir.getStats().fileCount == 1);
    }
}