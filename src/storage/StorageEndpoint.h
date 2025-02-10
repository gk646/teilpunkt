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
    ERR_NO_SUCH_ENDPOINT,
    ERR_UNSUPPORTED_OPERATION,
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
    UserID creator{};
};

struct StorageEndpoint
{
    explicit StorageEndpoint(const FileName& name, EndpointID endpoint);
    ~StorageEndpoint();

    //===== File Manipulation =====//

    StorageStatus addFile(UserID user, FileID dir, const FileCreationInfo& info, StorageTransaction& transaction);
    StorageStatus removeFile();
    StorageStatus changeFile();
    StorageStatus renameFile();

    StorageStatus addDirectory();
    StorageStatus removeDirectory();

    StorageStatus changeDirectory();
    StorageStatus renameDirectory();

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