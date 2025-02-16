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
    ERR_UNSUCCESSFUL,     // Generic error
    ERR_NO_AUTH,          // Invalid authentication
    ERR_NO_ADMIN,         // Requires admin but request doesnt have it
    ERR_NO_SUCH_ENDPOINT, // Endpoint not found
};

const char* StorageErrString(StorageStatus status);
int StorageErrCode(StorageStatus status);

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
    StorageEndpoint(const StorageEndpoint&) = delete;
    StorageEndpoint& operator=(const StorageEndpoint&) = delete;
    StorageEndpoint(StorageEndpoint&&) = default;
    StorageEndpoint& operator=(StorageEndpoint&&) = default;
    ~StorageEndpoint();

    //===== File Manipulation =====//

    StorageStatus fileAdd(UserID user, FileID dir, const FileCreationInfo& info, StorageTransaction& action);
    StorageStatus fileRemove();
    StorageStatus fileWrite();
    StorageStatus fileRename();
    StorageStatus fileGet(UserID user, FileID file, size_t begin, size_t end, ReadTransaction& action)
    {
        // Check if file exists - handle correct locking
        virtualFilesystem.fileExists(file);

        // Track actions - allow to view active actions per user

        // allow multiple reads - a closeWrite that doesn't revert closes all reads and then writes


        // Retrieve users key
        WrappedKey key;

        action = ReadTransaction();
        ReadHandle handle;
        if(!dataStore->initRead(file.file, begin, end, handle))
        {
            return StorageStatus::ERR_UNSUCCESSFUL;
        }



    }

    StorageStatus dirAdd();
    StorageStatus dirRemove();

    StorageStatus dirChange();
    StorageStatus dirRename();

    StorageStatus clear();

    //===== Storage Info =====//

    StorageStatus canBeAdded();

    EndpointID getID();

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