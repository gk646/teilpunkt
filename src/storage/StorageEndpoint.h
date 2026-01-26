// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include "datastructures/FixedString.h"
#include "datastructures/Spinlock.h"
#include "server/DTO.h"
#include "storage/datastore/DataStore.h"
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

//===== Error Handling =====//

enum class StorageStatus : uint8_t
{
    INVALID,
    OK,
    ERR_UNSUCCESSFUL,     // Generic error
    ERR_NO_UAC_PERM,      // No allowed by uac
    ERR_NO_ADMIN,         // Requires admin but request doesnt have it
    ERR_NO_SUCH_FILE,     // Specified files not found
    ERR_NO_SUCH_DIR,      // Specified directory not found
    ERR_INVALID_FILE_NAME,
    ERR_NO_UNIQUE_NAME,
    ERR_NO_SUCH_ENDPOINT, // Endpoint not found
};

const char* GetStorageStatusStr(StorageStatus status);

enum class StorageEndpointType : uint8_t
{
    LOCAL_FILE_SYSTEM,
    REMOTE_FILE_SYSTEM,
};

struct StorageEndpointCreateInfo final
{
    FileName name;
    uint64_t maxSize = 0; // Max size in bytes
    StorageEndpointType type{};
};

struct StorageEndpointData final
{
    StorageEndpointData(const StorageEndpointCreateInfo& info, UserID creator, EndpointID endpoint);

    FileName name;
    uint64_t maxSize = 0; // Max size in bytes
    StorageEndpointType type = StorageEndpointType::LOCAL_FILE_SYSTEM;
    UserID creator = UserID::INVALID;
    EndpointID endpoint = EndpointID::INVALID;
};

// Datastore stores its files in /endpoints/{id}
struct StorageEndpoint final
{
    StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID endpoint, UserID creator);
    ~StorageEndpoint() = default;
    TPUNKT_MACROS_DEL_CTORS(StorageEndpoint);

    //===== File Manipulation =====//

    StorageStatus fileCreate(UserID actor, FileID dir, const FileCreationInfo& info);
    StorageStatus fileWrite(UserID actor, FileID file, WriteFileTransaction& action);
    StorageStatus fileRead(UserID actor, FileID file, size_t begin, size_t end, ReadFileTransaction& action);
    StorageStatus fileRename(UserID actor, FileID file, const FileName& newName);

    //===== Dir Manipulation =====//

    StorageStatus dirCreate(UserID actor, FileID dir, const DirectoryCreationInfo& info);
    StorageStatus dirRemove();
    StorageStatus dirRename();

    // Collects all entries in the given dir (if user has access)
    StorageStatus dirGetEntries(UserID actor, FileID dir, std::vector<DTO::ResponseDirectoryEntry>& entries);

    //===== File Info =====//

    StorageStatus infoFile(UserID actor, FileID file, DTO::ResponseDirectoryEntry& info);
    StorageStatus infoDir(UserID actor, DTO::ResponseDirectoryInfo& info);

    //===== Storage Info =====//

    [[nodiscard]] const StorageEndpointData& getData() const;

    // True if no active usages
    [[nodiscard]] bool canBeRemoved() const;

    static bool CreateDirs(EndpointID eid);

  private:
    VirtualFilesystem virtualFilesystem;
    StorageEndpointData data;
    DataStore* dataStore = nullptr;
    Spinlock lock;
    friend Storage;
};

} // namespace tpunkt

#endif // TPUNKT_STORAGE_ENDPOINT_H
