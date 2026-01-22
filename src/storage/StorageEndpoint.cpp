// SPDX-License-Identifier: GPL-3.0-only
#include <cstdio>
#include "crypto/WrappedKey.h"
#include "datastructures/FixedString.h"
#include "storage/datastore/LocalFileSystem.h"
#include "storage/StorageEndpoint.h"
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
            return "No such storage endpoint";
        case StorageStatus::OK:
            return "OK";
        case StorageStatus::ERR_NO_SUCH_DIR:
            return "No such directory";
        case StorageStatus::ERR_NO_UNIQUE_NAME:
            return "No unique name";
    }
    return nullptr;
}

StorageEndpointData::StorageEndpointData(const StorageEndpointCreateInfo& info, UserID creator, EndpointID endpoint)
    : name(info.name), maxSize(info.maxSize), type(info.type), creator(creator), endpoint(endpoint)
{
}

StorageEndpoint::StorageEndpoint(const StorageEndpointCreateInfo& info, const EndpointID endpoint, const UserID creator)
    : virtualFilesystem(DirCreationInfo{
          .name = info.name, .creator = creator, .parent = FileID::Root(endpoint), .maxSize = info.maxSize}),
      data(info, creator, endpoint)
{
    switch(info.type)
    {
        case StorageEndpointType::LOCAL_FILE_SYSTEM:
            dataStore = new LocalFileSystemDatastore(endpoint);
            break;
        case StorageEndpointType::REMOTE_FILE_SYSTEM:
            LOG_CRITICAL("REMOTE_FILE_SYSTEM Not supported");
            break;
    }

    // TODO remove
    FileCreationInfo fileInfo{.name = "test.txt", .creator = UserID::SERVER, .endpoint = endpoint};
    fileCreate(UserID::SERVER, virtualFilesystem.getRoot().getID(), fileInfo);
}

StorageStatus StorageEndpoint::fileCreate(const UserID actor, const FileID dir, const FileCreationInfo& info)
{
    constexpr EventAction action = EventAction::FileSystemFileCreate;
    SpinlockGuard guard{lock};
    if(!IsValidFilename(info.name))
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_INVALID_FILE_NAME;
    }

    if(GetUAC().userCanAction(actor, dir, PermissionFlag::CREATE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* directory = virtualFilesystem.getDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    FileID newId{};
    if(!directory->fileAdd(info, newId))
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_NO_UNIQUE_NAME;
    }

    LOG_EVENT_FILESYS(actor, action, INFO_SUCCESS, FilesystemEventData{});
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

StorageStatus StorageEndpoint::fileRename(UserID user, FileID file, const FileName& newName)
{
}

StorageStatus StorageEndpoint::dirCreate(UserID actor, FileID dir, const DirCreationInfo& info)
{
    constexpr EventAction action = EventAction::FilesystemDirCreate;
    if(!IsValidFilename(info.name))
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_INVALID_FILE_NAME;
    }

    if(GetUAC().userCanAction(actor, dir, PermissionFlag::CREATE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* directory = virtualFilesystem.getDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    FileID newId{};
    if(!directory->dirAdd(info, newId))
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_NO_UNIQUE_NAME;
    }

    LOG_EVENT_FILESYS(actor, action, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::dirGetEntries(UserID actor, FileID dir, std::vector<DTO::DirectoryEntry>& entries)
{
    constexpr EventAction action = EventAction::FilesystemDirRead;
    SpinlockGuard guard{lock};
    if(GetUAC().userCanAction(actor, dir, PermissionFlag::READ) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    const VirtualDirectory* directory = virtualFilesystem.getDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, action, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    directory->collectEntries(entries);
    LOG_EVENT_FILESYS(actor, action, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

const StorageEndpointData& StorageEndpoint::getData() const
{
    return data;
}

bool StorageEndpoint::canBeRemoved() const
{
    return lock.isLocked();
}

bool StorageEndpoint::CreateDirs(EndpointID eid)
{
    if(!CreateRelDir(TPUNKT_STORAGE_ENDPOINT_DIR, true))
    {
        return false;
    }

    const auto endpointID = static_cast<int>(eid);
    FixedString<64> dir;
    (void)snprintf(dir.data(), dir.capacity(), "%s/%d", TPUNKT_STORAGE_ENDPOINT_DIR, endpointID);
    if(!CreateRelDir(dir.c_str(), true))
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
