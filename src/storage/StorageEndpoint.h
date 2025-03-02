// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>
#include "datastructures/FixedString.h"
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
    ERR_INVALID_FILE_NAME,
    ERR_NO_SUCH_ENDPOINT, // Endpoint not found
};

const char* StorageErrString(StorageStatus status);
int StorageHTTPErrCode(StorageStatus status);

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

// Datastore stores its files in /endpoints/{id}
struct StorageEndpoint
{
    explicit StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID eid, UserID creator, bool& success);
    TPUNKT_MACROS_MOVE_ONLY(StorageEndpoint);
    ~StorageEndpoint();

    //===== File Manipulation =====//

    StorageStatus fileCreate(UserID user, FileID dir, const FileCreationInfo& info, CreateFileTransaction& action);
    StorageStatus fileRemove();
    StorageStatus fileWrite();
    StorageStatus fileRename();
    StorageStatus fileRead(UserID user, FileID file, size_t begin, size_t end, ReadFileTransaction& action);

    //===== File Info =====//

    StorageStatus infoFile(User user, DTOFileInfo& info);
    StorageStatus infoDir(User user, DTODirectoryInfo& info);

    //===== Dir Manipulation =====//

    StorageStatus dirAdd();
    StorageStatus dirRemove();

    StorageStatus dirChange();
    StorageStatus dirRename();

    StorageStatus clear();

    //===== Storage Info =====//

    [[nodiscard]] EndpointID getID() const;

    [[nodiscard]] bool canBeRemoved() const;

  private:
    VirtualFilesystem virtualFilesystem;
    FixedString<64> dir; // Relative directory of the endpoint on disk
    FileName name;
    DataStore* dataStore = nullptr;
    EndpointID id;
    StorageEndpointType type{};
};

} // namespace tpunkt

#endif // TPUNKT_STORAGE_ENDPOINT_H