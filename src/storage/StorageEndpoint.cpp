// SPDX-License-Identifier: Apache License 2.0

#include <cerrno>
#include <cstdio>
#include <sys/stat.h>
#include "crypto/WrappedKey.h"
#include "datastructures/FixedString.h"
#include "storage/datastore/LocalFileSystem.h"
#include "storage/StorageEndpoint.h"
#include "storage/StorageTransaction.h"
#include "util/Logging.h"

namespace tpunkt
{

StorageEndpoint::StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID eid, UserID creator, bool& success)
    : virtualFilesystem({})
{
    if(!success)
    {
        return;
    }

    // Create the global storage endpoint dir - might exist
    if(mkdir(TPUNKT_STORAGE_ENDPOINT_DIR, TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
    {
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        success = false;
        return;
    }

    const auto endpointID = static_cast<int>(eid);
    (void)snprintf(dir.data(), dir.capacity(), "%s/%d", TPUNKT_STORAGE_ENDPOINT_DIR, endpointID);

    if(mkdir(dir.c_str(), TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
    {
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        success = false;
        return;
    }

    switch(info.type)
    {
        case StorageEndpointType::LOCAL_FILE_SYSTEM:
            dataStore = new LocalFileSystemDatastore(eid, success);
            break;
        case StorageEndpointType::REMOTE_FILE_SYSTEM:
            LOG_CRITICAL("Not supported");
            break;
    }
}

StorageStatus StorageEndpoint::fileAdd(UserID user, FileID dir, const FileCreationInfo& info,
                                       CreateTransaction& action)
{

}


StorageStatus StorageEndpoint::fileGet(UserID user, FileID file, size_t begin, size_t end)
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
    return;
}


} // namespace tpunkt