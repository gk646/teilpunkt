// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <sys/stat.h>
#include "crypto/WrappedKey.h"
#include "datastructures/FixedString.h"
#include "datastructures/SecureEraser.h"
#include "storage/datastore/LocalFileSystem.h"
#include "storage/StorageEndpoint.h"
#include "storage/StorageTransaction.h"
#include "uac/UserAccessControl.h"
#include "util/Logging.h"
#include "util/Strings.h"
#include "util/Wrapper.h"

namespace tpunkt
{

const char* GetStorageStatusStr(const StorageStatus status)
{
    switch(status)
    {
        case StorageStatus::INVALID:
            return "INVALID";
        case StorageStatus::ERR_UNSUCCESSFUL:
            return "Generic Error";
        case StorageStatus::ERR_NO_UAC_PERM:
            return "No access or file";
        case StorageStatus::ERR_NO_ADMIN:
            return "No admin";
        case StorageStatus::ERR_NO_SUCH_FILE:
            return "No access or file";
        case StorageStatus::ERR_INVALID_FILE_NAME:
            return "Invalid file name";
        case StorageStatus::ERR_NO_SUCH_ENDPOINT:
            return "No such endpoint";
        case StorageStatus::OK:
            return "OK";
    }
    return nullptr;
}

StorageEndpoint::StorageEndpoint(const StorageEndpointCreateInfo& info, const EndpointID endpoint, const UserID creator)
    : virtualFilesystem(DirectoryCreationInfo{info.name, info.maxSize, nullptr, creator}, endpoint)
{
    switch(info.type)
    {
        case StorageEndpointType::LOCAL_FILE_SYSTEM:
            dataStore = new LocalFileSystemDatastore(endpoint);
            break;
        case StorageEndpointType::REMOTE_FILE_SYSTEM:
            LOG_CRITICAL("Not supported");
            break;
    }
}

StorageEndpoint::~StorageEndpoint()
{
}

StorageStatus StorageEndpoint::fileCreate(const UserID actor, const FileID dir, const FileCreationInfo& info,
                                          CreateFileTransaction& action)
{
    if(GetUAC().userCanAction(actor, dir, PermissionFlag::CREATE) != UACStatus::OK)
    {
        LOG_EVENT(actor, Filesystem, FileSystemCreateFile, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* parent = virtualFilesystem.getDir(dir);

    if(parent == nullptr)
    {
        LOG_EVENT(actor, Filesystem, FileSystemCreateFile, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_FILE;
    }

    // CooperativeSpinlockGuard guard{parent->lock, false}; // Read lock

    if(parent->fileExists(info.name) || !IsValidFilename(info.name))
    {
        LOG_EVENT(actor, Filesystem, FileSystemCreateFile, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_INVALID_FILE_NAME;
    }

    new(&action) CreateFileTransaction{*dataStore, *parent, virtualFilesystem, info, dir};

    LOG_EVENT(actor, Filesystem, FileSystemCreateFile, INFO_SUCCESS, FilesystemEventData{});
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

StorageStatus StorageEndpoint::dirGetInfo(UserID user, FileID dir, std::vector<DTODirectoryEntry>& entries)
{
    entries.clear();

    entries.push_back({});
    entries.push_back({});

    return StorageStatus::OK;
}

const StorageEndpointInfo& StorageEndpoint::getInfo() const
{
    return info;
}

bool StorageEndpoint::CreateDirs(EndpointID eid)
{
    if(!CreateRelDir(TPUNKT_STORAGE_ENDPOINT_DIR, false))
    {
        return false;
    }

    const auto endpointID = static_cast<int>(eid);
    FixedString<64> dir;
    (void)snprintf(dir.data(), dir.capacity(), "%s/%d", TPUNKT_STORAGE_ENDPOINT_DIR, endpointID);
    if(!CreateRelDir(dir.c_str(), false))
    {
        RemoveRelDir(TPUNKT_STORAGE_ENDPOINT_DIR);
        return false;
    }

    if(!DataStore::CreateDirs(eid))
    {
        RemoveRelDir(dir.c_str());
        RemoveRelDir(TPUNKT_STORAGE_ENDPOINT_DIR);
        return false;
    }

    return true;
}


} // namespace tpunkt