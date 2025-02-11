// SPDX-License-Identifier: Apache License 2.0

#include <catch_amalgamated.hpp>
#include "storage/vfs/VirtualDirectory.h"
#include "TestCommons.h"

using namespace tpunkt;

uint32_t count = 1;
// Mocking or creating mock data
FileID getDirFileID()
{
    return FileID{.fileID = count++, .endpoint = EndpointID{0}, .isDirectory = true};
}

FileID getFileFileID()
{
    return FileID{.fileID = count++, .endpoint = EndpointID{0}, .isDirectory = false};
}

UserID generateUserID()
{
    return UserID{54321};
}

TEST_CASE("DirectoryCreationInfo")
{
    TEST_INIT();

    SECTION("Default constructor")
    {
        DirectoryCreationInfo dirInfo;
        REQUIRE(dirInfo.maxSize == 0);
        REQUIRE(dirInfo.parent == nullptr);
        REQUIRE(dirInfo.creator == UserID::INVALID);
    }

    SECTION("Custom constructor")
    {
        DirectoryCreationInfo dirInfo{FileName{"TestDirectory"}, 1000U, nullptr, getDirFileID(), generateUserID()};

        REQUIRE(dirInfo.name == "TestDirectory");
        REQUIRE(dirInfo.maxSize == 1000);
        REQUIRE(dirInfo.creator != UserID::INVALID);
    }
}

TEST_CASE("VirtualDirectory")
{
    TEST_INIT();

    DirectoryCreationInfo rootDirInfo{FileName{"RootDirectory"}, 1000, nullptr, getDirFileID(), generateUserID()};

    VirtualDirectory rootDir(rootDirInfo);

    SECTION("Directory creation")
    {
        DirectoryCreationInfo subDirInfo{FileName{"SubDirectory"}, 500, &rootDir, getDirFileID(), generateUserID()};
        bool added = rootDir.addDirectory(subDirInfo);
        REQUIRE(added == true);
        REQUIRE(rootDir.getDirCount() == 1);                  // Subdirectory is added
    }

    SECTION("Directory removal")
    {
        DirectoryCreationInfo subDirInfo{FileName{"SubDirectoryToRemove"}, 500, &rootDir, getDirFileID(),
                                         generateUserID()};
        rootDir.addDirectory(subDirInfo);
        bool removed = rootDir.removeDirectory(subDirInfo.id);
        REQUIRE(removed == true);
        REQUIRE(rootDir.getDirCount() == 0);                  // Subdirectory is removed
    }

    SECTION("File addition")
    {
        FileCreationInfo fileInfo{FileName{"File1"}, generateUserID(), 200, getFileFileID()};
        const bool fits = rootDir.canFit(fileInfo.size);
        const bool unique = !rootDir.nameExists(fileInfo.name);
        bool added = rootDir.addFile(fileInfo, fits, unique); // fits = true, unique = true
        REQUIRE(added == true);
        REQUIRE(rootDir.getFileCount() == 1);                 // File is added
    }

    SECTION("File removal")
    {
        FileCreationInfo fileInfo{FileName{"FileToRemove"}, generateUserID(), 200, getFileFileID()};
        const bool fits = rootDir.canFit(fileInfo.size);
        const bool unique = !rootDir.nameExists(fileInfo.name);
        rootDir.addFile(fileInfo, fits, unique);
        bool removed = rootDir.removeFile(fileInfo.id);
        REQUIRE(removed == true);
        REQUIRE(rootDir.getFileCount() == 0);                 // File is removed
    }

    SECTION("Can fit check")
    {
        FileCreationInfo fileInfo{FileName{"LargeFile"}, generateUserID(), 800, getFileFileID()};
        bool canFit = rootDir.canFit(fileInfo.size);
        REQUIRE(canFit == true);                              // Assuming rootDir has sufficient space
    }

    SECTION("Name exists check")
    {
        FileCreationInfo fileInfo{FileName{"FileNameCheck"}, generateUserID(), 300, getFileFileID()};
        const bool fits = rootDir.canFit(fileInfo.size);
        const bool unique = !rootDir.nameExists(fileInfo.name);
        rootDir.addFile(fileInfo, fits, unique);
        bool exists = rootDir.nameExists(FileName{"FileNameCheck"});
        REQUIRE(exists == true);

        exists = rootDir.nameExists(FileName{"NonExistentFile"});
        REQUIRE(exists == false);
    }
}

TEST_CASE("Edge cases")
{
    TEST_INIT();

    VirtualDirectory rootDir(DirectoryCreationInfo{FileName{"Root"}, 1000, nullptr, getDirFileID(), generateUserID()});

    SECTION("Adding file when directory is at max size")
    {
        FileCreationInfo fileInfo{FileName{"LargeFile"}, generateUserID(), 2000, getFileFileID()};

        bool canFit = rootDir.canFit(fileInfo.size);
        REQUIRE(canFit == false);                             // File cannot fit

        bool added = rootDir.addFile(fileInfo, canFit, true); // Trying to add a file that doesn't fit
        REQUIRE(added == false);                              // Should fail
    }

    SECTION("Adding a file with duplicate name")
    {
        FileCreationInfo fileInfo1{FileName{"DuplicateFile"}, generateUserID(), 100, getFileFileID()};
        FileCreationInfo fileInfo2{FileName{"DuplicateFile"}, generateUserID(), 200, getFileFileID()};

        bool fits = rootDir.canFit(fileInfo1.size);
        bool unique = !rootDir.nameExists(fileInfo1.name);

        REQUIRE(rootDir.addFile(fileInfo1, fits, unique));
        fits = rootDir.canFit(fileInfo1.size);
        unique = !rootDir.nameExists(fileInfo1.name);
        bool added = rootDir.addFile(fileInfo2, fits, unique); // Trying to add a file with the same name
        REQUIRE(added == false);                               // Should fail because name is not unique
    }
}

TEST_CASE("File creation and removal")
{
    TEST_INIT();

    SECTION("File creation")
    {
        FileCreationInfo fileInfo{FileName{"TestFile"}, generateUserID(), 500, getFileFileID()};
        REQUIRE(fileInfo.name == "TestFile");
        REQUIRE(fileInfo.size == 500);
    }

    SECTION("File removal failure")
    {
        VirtualDirectory dir(DirectoryCreationInfo{FileName{"Root"}, 1000, nullptr, getDirFileID(), generateUserID()});

        FileID nonExistentFileID{99999};
        bool removed = dir.removeFile(nonExistentFileID);
        REQUIRE(removed == false); // Trying to remove a non-existent file
    }
}

TEST_CASE("Size Limit")
{
    TEST_INIT();
    VirtualDirectory rootDir(DirectoryCreationInfo{FileName{"Root"}, 1000, nullptr, getDirFileID(), generateUserID()});

    uint32_t firstID = count;
    for(int i = 0; i < 10; ++i)
    {
        FileName name{(("Sub1" + std::to_string(i)).c_str())};
        DirectoryCreationInfo dir{name, 600, &rootDir, getDirFileID(), generateUserID()};
        rootDir.addDirectory(dir);
    }

    FileCreationInfo fileInfo{FileName{"TestFile"}, generateUserID(), 500, getFileFileID()};

    for(int i = 0; i < 2; ++i)
    {
        VirtualDirectory* dir = rootDir.searchDir({firstID++, EndpointID{0}, true});
        REQUIRE(dir != nullptr);
        const bool fits = rootDir.canFit(fileInfo.size);
        const bool unique = !rootDir.nameExists(fileInfo.name);
        REQUIRE(dir->addFile(fileInfo, fits, unique));
    }

    // Third should fail
    {
        VirtualDirectory* dir1 = rootDir.searchDir({firstID, EndpointID{0}, true});
        REQUIRE(dir1 != nullptr);
        const bool fits = rootDir.canFit(fileInfo.size);
        const bool unique = !rootDir.nameExists(fileInfo.name);
        REQUIRE_FALSE(fits);
        REQUIRE_FALSE(dir1->addFile(fileInfo, fits, unique));
    }
}