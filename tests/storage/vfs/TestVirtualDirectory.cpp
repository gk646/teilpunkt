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

DirectoryCreationInfo getDirInfo(const char* name, const uint64_t maxSize, VirtualDirectory* parent)
{
    return {.name = FileName{name}, .maxSize = maxSize, .parent = parent, .creator = getUserID()};
}

void CheckAccess(const VirtualDirectory& dir, const DirectoryStats& previous, const bool happened)
{
    if(happened)
    {
        REQUIRE(dir.getStats().accessCount == previous.accessCount + 2); // getStats is also a access
        REQUIRE(previous.lastAccess < dir.getStats().lastAccess);
    }
    else
    {
        REQUIRE(dir.getStats().accessCount == previous.accessCount);
        REQUIRE(previous.lastAccess == dir.getStats().lastAccess);
    }
}

void CheckChange(const VirtualDirectory& dir, const DirectoryStats& previous, const bool happened)
{
    if(happened)
    {
        REQUIRE(dir.getStats().changeCount == previous.changeCount + 1); // getStats is also a access
        REQUIRE(previous.lastEdit < dir.getStats().lastEdit);
    }
    else
    {
        REQUIRE(dir.getStats().changeCount == previous.changeCount);
        REQUIRE(previous.lastEdit == dir.getStats().lastEdit);
    }
}

TEST_CASE("VirtualDirectory Simple File Operations")
{
    TEST_INIT();
    constexpr auto sizeLimit = 1000;
    auto baseDir = VirtualDirectory{getDirInfo("Base Dir", sizeLimit, nullptr), EndpointID{1}};

    {
        REQUIRE(baseDir.getStats().accessCount == 1); // getStats() is access
        REQUIRE(baseDir.getStats().changeCount == 0);

        REQUIRE(baseDir.getStats().lastAccess.isInPast());
        REQUIRE(baseDir.getStats().lastEdit.isInPast());
        REQUIRE(baseDir.getStats().creation.isInPast());

        REQUIRE(baseDir.getLimits().sizeLimit == sizeLimit);
    }

    FileID file1;
    {
        const DirectoryStats stats = baseDir.getStats();
        auto result = baseDir.fileAdd(getFileInfo("File1"), file1);
        REQUIRE(result);
        CheckAccess(baseDir, stats, true);
        CheckChange(baseDir, stats, true);
        REQUIRE(baseDir.getStats().fileCount == 1);
    }

    // Add existing file
    {
        const DirectoryStats stats = baseDir.getStats();
        auto result = baseDir.fileAdd(getFileInfo("File1"), file1);
        REQUIRE_FALSE(result);
        CheckAccess(baseDir, stats, true); // Still accessed
        CheckChange(baseDir, stats, false);
    }

    // Change within limit bigger
    {
        const DirectoryStats stats = baseDir.getStats();
        auto result = baseDir.fileChange(file1, 100);
        REQUIRE(result);

        CheckAccess(baseDir, stats, true);
        CheckChange(baseDir, stats, true);
        REQUIRE(baseDir.getStats().fileSize == 100);
    }

    // Change outside limit
    {
        const DirectoryStats stats = baseDir.getStats();
        auto result = baseDir.fileChange(file1, 1000);
        REQUIRE_FALSE(result);
        CheckAccess(baseDir, stats, true);
        CheckChange(baseDir, stats, false);
        REQUIRE(baseDir.getStats().fileSize == 100);
    }

    // Change within limit smaller
    {
        const DirectoryStats stats = baseDir.getStats();
        auto result = baseDir.fileChange(file1, 10);
        REQUIRE(result);
        CheckAccess(baseDir, stats, true);
        CheckChange(baseDir, stats, true);
        REQUIRE(baseDir.getStats().fileSize == 10);
    }

    // ("Exists")
    {
        {
            const DirectoryStats stats = baseDir.getStats();
            auto result = baseDir.fileExists("File1");
            REQUIRE(result);
            CheckAccess(baseDir, stats, true);
            CheckChange(baseDir, stats, false);
        }

        {
            const DirectoryStats stats = baseDir.getStats();
            auto result = baseDir.fileExists("File111");
            REQUIRE_FALSE(result);
            CheckAccess(baseDir, stats, true);
            CheckChange(baseDir, stats, false);
        }
    }

    // ("Remove")
    {
        // Remove existing
        {
            const DirectoryStats stats = baseDir.getStats();
            auto result = baseDir.fileRemove(file1);
            REQUIRE(result);
            CheckAccess(baseDir, stats, true);
            CheckChange(baseDir, stats, true);
            REQUIRE(baseDir.getStats().fileCount == 0);
        }

        // Remove missing
        {
            const DirectoryStats stats = baseDir.getStats();
            auto result = baseDir.fileRemove(file1);
            REQUIRE_FALSE(result);
            CheckAccess(baseDir, stats, true);
            CheckChange(baseDir, stats, false);
        }
    }
}