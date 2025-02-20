// SPDX-License-Identifier: Apache License 2.0

#include <catch_amalgamated.hpp>
#include <filesystem>
#include "storage/datastore/stores/LocalFileSystem.h"
#include "TestCommons.h"

using namespace tpunkt;

namespace fs = std::filesystem;
std::string baseDir = "./endpoints";
std::string testDir = "./endpoints/1/datastore/";

TEST_CASE("Local File System")
{
    fs::create_directories("./endpoints/1/datastore"); // Create directories
    TEST_INIT();
    DataStore* store = new LocalFileSystemDatastore(EndpointID{1});

    SECTION("File Creation")
    {
        uint32_t fileID = 12345;
        std::string filePath = testDir + std::to_string(fileID);

        REQUIRE_FALSE(fs::exists(filePath));

        bool createSuccess = false;
        store->createFile(fileID, [ & ](bool success) { createSuccess = success; });

        REQUIRE(createSuccess);
        REQUIRE(fs::exists(filePath));

        bool deleteSuccess = false;
        store->deleteFile(fileID, [ & ](bool success) { deleteSuccess = success; });

        REQUIRE(deleteSuccess);
        REQUIRE_FALSE(fs::exists(filePath));
    }

    SECTION("Write and read operations")
    {
        uint32_t fileID = 67890;
        std::string filePath = testDir + std::to_string(fileID);
        WriteHandle writeHandle;
        ReadHandle readHandle;

        // Create file before writing
        store->createFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE(store->initWrite(fileID, writeHandle));

        const char testData[] = "Hello, world!";
        bool writeSuccess = false;
        store->writeFile(writeHandle, true, reinterpret_cast<const unsigned char*>(testData), sizeof(testData),
                         [ & ](bool success) { writeSuccess = success; });

        REQUIRE(writeSuccess);
        REQUIRE(store->closeWrite(writeHandle, false, [](bool success) { REQUIRE(success); }));

        REQUIRE(store->initRead(fileID, 0, 0, readHandle));

        bool readSuccess = false;
        store->readFile(readHandle, sizeof(testData),
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(isLast);
                            REQUIRE(size == sizeof(testData));
                            REQUIRE(std::memcmp(data, testData, size) == 0);
                            readSuccess = true;
                        });

        REQUIRE(readSuccess);
        REQUIRE(store->closeRead(readHandle, [](bool success) { REQUIRE(success); }));

        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });
    }

    SECTION("Error Handling - Read from non-existent file")
    {
        uint32_t fileID = 99999;
        ReadHandle handle;

        REQUIRE_FALSE(store->initRead(fileID, 0, 0, handle));
    }

    SECTION("Error Handling - Write to deleted file")
    {
        uint32_t fileID = 54321;
        WriteHandle handle;

        store->createFile(fileID, [](bool success) { REQUIRE(success); });
        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE_FALSE(store->initWrite(fileID, handle));
    }

    SECTION("Error Handling - Revert a write operation")
    {
        uint32_t fileID = UINT32_MAX;
        WriteHandle writeHandle{};

        store->createFile(fileID, [](bool success) { REQUIRE(success); });

        auto var = testDir + std::to_string(fileID);
        REQUIRE(fs::exists(testDir + std::to_string(fileID)));

        REQUIRE(store->initWrite(fileID, writeHandle));

        const char testData[] = "This should be discarded.";
        store->writeFile(writeHandle, true, reinterpret_cast<const unsigned char*>(testData), sizeof(testData),
                         [](bool success) { REQUIRE(success); });

        REQUIRE(store->closeWrite(writeHandle, true, [](bool success) { REQUIRE(success); }));

        REQUIRE(fs::exists(testDir + std::to_string(fileID)));

        ReadHandle readHandle;
        REQUIRE(store->initRead(fileID, 0, 0, readHandle));

        bool dataRead = false;
        store->readFile(readHandle, 1024,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(size == 0);
                            REQUIRE(isLast);
                            dataRead = true;
                        });

        REQUIRE(dataRead);

        REQUIRE(store->closeRead(readHandle, [](bool success) { REQUIRE(success); }));

        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE_FALSE(fs::exists(testDir + std::to_string(fileID)));
    }


    SECTION("Multiple Sequential Writes and Reads")
    {
        uint32_t fileID = 424242;
        std::string filePath = testDir + std::to_string(fileID);
        WriteHandle writeHandle;
        ReadHandle readHandle;

        store->createFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE(store->initWrite(fileID, writeHandle));

        const char firstChunk[] = "First chunk. ";
        const char secondChunk[] = "Second chunk.";

        bool writeSuccess = false;
        store->writeFile(writeHandle, false, reinterpret_cast<const unsigned char*>(firstChunk), sizeof(firstChunk) - 1,
                         [ & ](bool success) { writeSuccess = success; });
        REQUIRE(writeSuccess);

        store->writeFile(writeHandle, true, reinterpret_cast<const unsigned char*>(secondChunk),
                         sizeof(secondChunk) - 1, [ & ](bool success) { writeSuccess = success; });
        REQUIRE(writeSuccess);

        REQUIRE(store->closeWrite(writeHandle, false, [](bool success) { REQUIRE(success); }));

        REQUIRE(store->initRead(fileID, 0, 0, readHandle));

        bool readFirstChunk = false;
        store->readFile(readHandle, sizeof(firstChunk) - 1,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE_FALSE(isLast);
                            REQUIRE(size == sizeof(firstChunk) - 1);
                            REQUIRE(std::memcmp(data, firstChunk, size) == 0);
                            readFirstChunk = true;
                        });
        REQUIRE(readFirstChunk);

        bool readSecondChunk = false;
        store->readFile(readHandle, sizeof(secondChunk) - 1,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(size == sizeof(secondChunk) - 1);
                            REQUIRE(std::memcmp(data, secondChunk, size) == 0);
                            REQUIRE(isLast);
                            readSecondChunk = true;
                        });
        REQUIRE(readSecondChunk);

        REQUIRE(store->closeRead(readHandle, [](bool success) { REQUIRE(success); }));

        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });
    }

    SECTION("Mid-File Reads")
    {
        uint32_t fileID = 505050;
        std::string filePath = testDir + std::to_string(fileID);
        WriteHandle writeHandle;
        ReadHandle readHandle;

        store->createFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE(store->initWrite(fileID, writeHandle));

        const char testData[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 36 chars

        store->writeFile(writeHandle, true, reinterpret_cast<const unsigned char*>(testData), sizeof(testData) - 1,
                         [](bool success) { REQUIRE(success); });

        REQUIRE(store->closeWrite(writeHandle, false, [](bool success) { REQUIRE(success); }));

        REQUIRE(store->initRead(fileID, 10, 26, readHandle));

        bool readSuccess = false;
        store->readFile(readHandle, 16,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(size == 16);
                            REQUIRE(memcmp(data, "ABCDEFGHIJKLMNOP", size) == 0);
                            REQUIRE(isLast);
                            readSuccess = true;
                        });

        REQUIRE(readSuccess);
        REQUIRE(store->closeRead(readHandle, [](bool success) { REQUIRE(success); }));

        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });
    }

    SECTION("Handling Empty Files")
    {
        uint32_t fileID = 606060;
        std::string filePath = testDir + std::to_string(fileID);
        ReadHandle readHandle;

        store->createFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE(store->initRead(fileID, 0, 0, readHandle));

        bool readEmpty = false;
        store->readFile(readHandle, 1024,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(size == 0);
                            REQUIRE(isLast);
                            readEmpty = true;
                        });

        REQUIRE(readEmpty);
        REQUIRE(store->closeRead(readHandle, [](bool success) { REQUIRE(success); }));

        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });
    }

    SECTION("Read Partial Chunks")
    {
        uint32_t fileID = 707070;
        std::string filePath = testDir + std::to_string(fileID);
        WriteHandle writeHandle;
        ReadHandle readHandle;

        store->createFile(fileID, [](bool success) { REQUIRE(success); });

        REQUIRE(store->initWrite(fileID, writeHandle));

        const char testData[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 26 bytes

        store->writeFile(writeHandle, true, reinterpret_cast<const unsigned char*>(testData), sizeof(testData) - 1,
                         [](bool success) { REQUIRE(success); });

        REQUIRE(store->closeWrite(writeHandle, false, [](bool success) { REQUIRE(success); }));

        REQUIRE(store->initRead(fileID, 0, 0, readHandle));

        bool readFirstChunk = false;
        store->readFile(readHandle, 10,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(size == 10);
                            REQUIRE_FALSE(isLast);
                            REQUIRE(std::memcmp(data, "ABCDEFGHIJ", size) == 0);
                            readFirstChunk = true;
                        });
        REQUIRE(readFirstChunk);

        bool readSecondChunk = false;
        store->readFile(readHandle, 10,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE_FALSE(isLast);
                            REQUIRE(size == 10);
                            REQUIRE(std::memcmp(data, "KLMNOPQRST", size) == 0);
                            readSecondChunk = true;
                        });
        REQUIRE(readSecondChunk);

        bool readLastChunk = false;
        store->readFile(readHandle, 6,
                        [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
                        {
                            REQUIRE(success);
                            REQUIRE(size == 6);
                            REQUIRE(std::memcmp(data, "UVWXYZ", size) == 0);
                            REQUIRE(isLast);
                            readLastChunk = true;
                        });
        REQUIRE(readLastChunk);

        REQUIRE(store->closeRead(readHandle, [](bool success) { REQUIRE(success); }));

        store->deleteFile(fileID, [](bool success) { REQUIRE(success); });
    }


    fs::remove_all(baseDir);
}