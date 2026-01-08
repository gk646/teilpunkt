// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>
#include "datastructures/FixedString.h"
#include "server/DTO.h"
#include "storage/datastore/DataStore.h"
#include "storage/StorageTransaction.h"
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
    FileName name;
    uint64_t maxSize = 0; // Max size in bytes
    StorageEndpointType type = StorageEndpointType::LOCAL_FILE_SYSTEM;
    UserID creator = UserID::INVALID;
    EndpointID eid = EndpointID::INVALID;
};

// Datastore stores its files in /endpoints/{id}
struct StorageEndpoint final
{
     StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID endpoint, UserID creator);
    TPUNKT_MACROS_MOVE_ONLY(StorageEndpoint);
    ~StorageEndpoint();

    //===== File Manipulation =====//

    StorageStatus fileCreate(UserID actor, FileID dir, const FileCreationInfo& info, CreateFileTransaction& action);
    StorageStatus fileWrite(UserID user, FileID file, WriteFileTransaction& action);
    StorageStatus fileRead(UserID user, FileID file, size_t begin, size_t end, ReadFileTransaction& action);
    StorageStatus fileRemove(UserID user, FileID file);
    StorageStatus fileRename(UserID user, FileID file, const FileName& newName);

    //===== File Info =====//

    StorageStatus infoFile(User user, DTOFileInfo& info);
    StorageStatus infoDir(User user, DTODirectoryInfo& info);

    //===== Dir Manipulation =====//

    StorageStatus dirAdd();
    StorageStatus dirRemove();

    StorageStatus dirChange();
    StorageStatus dirRename();

    // Collects all files in the given dir the user has access to
    StorageStatus dirGetInfo(UserID user, FileID dir, std::vector<DTODirectoryEntry>& entries);

    StorageStatus clear();

    //===== Storage Info =====//

    [[nodiscard]] const StorageEndpointData& getInfo() const;

    // True if no active usages
    [[nodiscard]] bool canBeRemoved() const;

    static bool CreateDirs(EndpointID eid);

  private:
    VirtualFilesystem virtualFilesystem;
    StorageEndpointData data;
    DataStore* dataStore = nullptr;
    Spinlock lock;
};

} // namespace tpunkt

#endif // TPUNKT_STORAGE_ENDPOINT_H
