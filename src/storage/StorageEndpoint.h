// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>
#include "datastructures/FixedString.h"
#include "storage/datastore/DataStore.h"
#include "storage/datastore/StorageTransaction.h"
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{
enum class StorageStatus : uint8_t
{
    INVALID,
    OK,
    ERR_GENERIC,          // Generic error
    ERR_NO_AUTH,          // Invalid authentication
    ERR_NO_ADMIN,         // Requires admin but request doesnt have it
    ERR_NO_PERMS,         // File access denied by local filesystem
    ERR_NO_SUCH_ENDPOINT, // Endpoint not found
};

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

struct StorageEndpoint
{
    explicit StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID eid, UserID creator, bool& success);
    ~StorageEndpoint();

    //===== File Manipulation =====//

    StorageStatus fileAdd(UserID user, FileID dir, const FileCreationInfo& info, StorageTransaction& transaction);
    StorageStatus fileRemove();
    StorageStatus changeFile();
    StorageStatus renameFile();

    StorageStatus dirAdd();
    StorageStatus dirRemove();

    StorageStatus dirChange();
    StorageStatus dirRename();

    StorageStatus clear();

    //===== Storage Info =====//

    StorageStatus canBeAdded();

  private:
    VirtualFilesystem virtualFilesystem;
    DataStore* dataStore = nullptr;
    FileName name;
    EndpointID id;
    StorageEndpointType type{};
    FixedString<32> dir; // Physical directory of the endpoint
};

} // namespace tpunkt

#endif // TPUNKT_STORAGE_ENDPOINT_H