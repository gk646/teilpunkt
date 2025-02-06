#ifndef TPUNKT_STORAGE_H
#define TPUNKT_STORAGE_H

#include <vector>
#include "storage/StorageEndpoint.h"
#include "datastructures/Spinlock.h"
#include "server/DTO.h"

namespace tpunkt
{
    // All write operations are atomic across a single endpoint
    // Implicitly checks permissions using the UAC
    struct Storage final
    {
        //===== Global Functions =====//

        // Get all the roots of all directories/files this user has access to
        StorageStatus userGetRoots(UserID user, std::vector<DirectoryInfoDTO>& roots);

        // Collects all files in the given directory
        StorageStatus userGetDir(UserID user, FileID dir, std::vector<DirectoryEntryDTO>& entries);


        //===== Endpoint Management =====//

        StorageStatus getEndpoint(StorageEndpoint* endpoint);

      private:
        FileID getNextFileID(bool isDirectory, uint8_t endPoint);

        std::vector<StorageEndpoint> endpoints;
        Spinlock storageLock;
        uint32_t fileID = 0;
        uint8_t endpoint = 0;
        friend StorageTransaction;
    };

    // Can only access storage with valid token
    Storage* GetStorage();

} // namespace tpunkt

#endif // TPUNKT_STORAGE_H