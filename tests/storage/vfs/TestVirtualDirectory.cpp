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

VirtualFile getFileInfo(const FileName& name)
{
    return VirtualFile{{.name = name, .creator = getUserID()}};
}


VirtualDirectory getDir(const char* name, uint64_t maxSize, VirtualDirectory* parent)
{
    return VirtualDirectory{{.name = FileName{name}, .maxSize = maxSize, .parent = parent, .creator=getUserID()}};
}


TEST_CASE("Virtual Directory")
{
    TEST_INIT();

    auto baseDir = getDir("Base Dir", 1000, nullptr);

    SECTION("Add File")
    {
        //baseDir.fileAdd()
    }
}