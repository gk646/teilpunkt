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
    return {.name = FileName{name}, .maxSize = maxSize, .parent = parent, getUserID()};
}


VirtualDirectory getDir(const char* name)
{

    return VirtualDirectory{};
}

TEST_CASE("Virtual Directory")
{
    TEST_INIT();


    SECTION("Add File")
    {
        VirtualDirectory dir{};
    }
}