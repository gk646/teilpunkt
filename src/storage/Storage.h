// SPDX-License-Identifier: Apache License 2.0

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
    //===== Global Functions =====//

    // If user is different from token-user requires admin
    StorageStatus getRoots(const AuthToken& token, UserID user, std::vector<DirectoryInfoDTO>& roots);

    // If user is different from token-user requires admin
    StorageStatus getDir(const AuthToken& token, UserID user, FileID dir, std::vector<DirectoryEntryDTO>& entries);

    //===== Endpoint Management =====//

    using CreateInfo = const StorageEndpointCreateInfo&;

    // Creates a new empty endpoint
    StorageStatus endpointCreate(const AuthToken& token, CreateInfo info);

    // Creates a new endpoint by copying from the given local file source - requires admin && process needs access
    StorageStatus endpointCreateFrom(const AuthToken& token, CreateInfo info, const char* file, bool recurse);

    // Only valid if returns StorageStatus::OK
    StorageStatus endpointGet(const AuthToken& token, EndpointID endpointId, StorageEndpoint*& endpoint);

    // Deletes the given endpoint
    StorageStatus endpointDelete(const AuthToken& token, EndpointID endpointId);

  private:
    FileID getNextFile(bool isDirectory, EndpointID endPoint);
    EndpointID getEndpointID(bool increment);

    std::vector<StorageEndpoint> endpoints;
    Spinlock storageLock;
    uint32_t fileID = 0;
    uint8_t endpoint = 0;
    friend StorageTransaction;
};

Storage& GetStorage();

} // namespace tpunkt

#endif // TPUNKT_STORAGE_H