// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_STORAGE_H
#define TPUNKT_STORAGE_H

#include <vector>
#include "datastructures/Spinlock.h"
#include "server/DTO.h"
#include "storage/StorageEndpoint.h"

namespace tpunkt
{

// Implicitly checks permissions using the UAC
struct Storage final
{

    Storage();
    ~Storage();

    //===== Global Functions =====//

    StorageStatus getRoots(UserID user, std::vector<DTODirectoryInfo>& roots);

    StorageStatus getDir(UserID user, FileID dir, std::vector<DTODirectoryEntry>& entries);

    // Also works across endpoints
    StorageStatus move(UserID user, const std::vector<FileID>& files, FileID dest);

    // Also works across endpoints
    StorageStatus copy(UserID user, const std::vector<FileID>& files, FileID dest);

    //===== Endpoint Management =====//

    using CreateInfo = const StorageEndpointCreateInfo&;

    // Creates a new empty endpoint
    StorageStatus endpointCreate(UserID user, CreateInfo info);

    // Creates a new endpoint by copying from the given local file source - requires admin && process needs access
    StorageStatus endpointCreateFrom(UserID user, CreateInfo info, const char* file, bool recurse);

    // Only valid if returns StorageStatus::OK
    StorageStatus endpointGet(UserID user, EndpointID endpointId, StorageEndpoint*& ept);

    // Deletes the given endpoint
    StorageStatus endpointDelete(UserID user, EndpointID endpoint);

    // Returns the next file number
    uint32_t getNextID();

  private:
    std::forward_list<StorageEndpoint, SharedBlockAllocator<StorageEndpoint>> endpoints;
    Spinlock storageLock;
    Spinlock fileIDLock;
    uint32_t endpointID = 0;
    uint32_t filesID = 1;
};

Storage& GetStorage();

} // namespace tpunkt

#endif // TPUNKT_STORAGE_H