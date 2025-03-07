// SPDX-License-Identifier: GPL-3.0-only

#include <cerrno>
#include <cstdio>
#include <sys/stat.h>
#include "crypto/WrappedKey.h"
#include "datastructures/FixedString.h"
#include "storage/datastore/LocalFileSystem.h"
#include "storage/StorageEndpoint.h"
#include "storage/StorageTransaction.h"
#include "uac/UserAccessControl.h"
#include "util/Logging.h"
#include "util/Strings.h"

namespace tpunkt
{

StorageEndpoint::StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID eid, UserID creator)
    : virtualFilesystem({})
{
    FixedString<64> dir;

    // Create the global storage endpoint dir - might exist
    if(mkdir(TPUNKT_STORAGE_ENDPOINT_DIR, TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
    {
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        return;
    }

    const auto endpointID = static_cast<int>(eid);
    (void)snprintf(dir.data(), dir.capacity(), "%s/%d", TPUNKT_STORAGE_ENDPOINT_DIR, endpointID);

    if(mkdir(dir.c_str(), TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
    {
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        return;
    }

    switch(info.type)
    {
        case StorageEndpointType::LOCAL_FILE_SYSTEM:
            dataStore = new LocalFileSystemDatastore(eid);
            break;
        case StorageEndpointType::REMOTE_FILE_SYSTEM:
            LOG_CRITICAL("Not supported");
            break;
    }
}

StorageStatus StorageEndpoint::fileCreate(const UserID user, const FileID dir, const FileCreationInfo& info,
                                          CreateFileTransaction& action)
{
    if(GetUAC().userCanAction(user, dir, PermissionFlag::CREATE) != UACStatus::OK)
    {
        LOG_EVENT(UserAction, FilesystemCreateFile, FAIL_NO_UAC);
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* parent = virtualFilesystem.getDir(dir);

    if(parent == nullptr)
    {
        LOG_EVENT(UserAction, FilesystemCreateFile, FAIL_NO_SUCH_FILE);
        return StorageStatus::ERR_NO_SUCH_FILE;
    }

    CooperativeSpinlockGuard guard{parent->lock, false}; // Read lock

    if(parent->fileExists(info.name) || !IsValidFilename(info.name))
    {
        LOG_EVENT(UserAction, FilesystemCreateFile, FAIL_INVALID_ARGUMENTS);
        return StorageStatus::ERR_INVALID_FILE_NAME;
    }

    new(&action) CreateFileTransaction{*dataStore, *parent, virtualFilesystem, info, dir};

    LOG_EVENT(UserAction, FilesystemCreateFile, SUCCESS);
    return StorageStatus::OK;
}


StorageStatus StorageEndpoint::fileRead(UserID user, FileID file, size_t begin, size_t end, ReadFileTransaction& action)
{
    // Check if file exists - handle correct locking
    // virtualFilesystem.fileExists(file);

    // Track actions - allow to view active actions per user

    // allow multiple reads - a closeWrite that doesn't revert closes all reads and then writes

    // Retrieve users key
    WrappedKey key;

    // action = ReadTransaction();
    ReadHandle handle;
    // if(!dataStore->initRead(file.file, begin, end, handle))
    {
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    return StorageStatus::OK;
}


} // namespace tpunkt