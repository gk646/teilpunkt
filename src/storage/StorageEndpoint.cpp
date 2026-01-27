// SPDX-License-Identifier: GPL-3.0-only
#include <cstdio>
#include "crypto/WrappedKey.h"
#include "storage/StorageTransaction.h"
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
    : virtualFilesystem(DirectoryCreationInfo{
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
    FileID newFile{};
    fileCreate(UserID::SERVER, virtualFilesystem.getRoot().getID(), fileInfo, newFile);
}

StorageStatus StorageEndpoint::fileCreate(UserID actor, FileID dir, const FileCreationInfo& info, FileID& newFile)
{
    constexpr EventAction action = EventAction::FileSystemFileCreate;
    SpinlockGuard guard{lock};
    if(!IsValidFilename(info.name))
    {
        LOG_EVENT_FILESYS(actor, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_INVALID_FILE_NAME;
    }

    if(GetUAC().userCanAction(actor, dir, PermissionFlag::CREATE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* directory = virtualFilesystem.findDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    if(!directory->fileAdd(info, newFile))
    {
        LOG_EVENT_FILESYS(actor, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_NO_UNIQUE_NAME;
    }

    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::fileDelete(UserID actor, FileID file)
{
    constexpr EventAction action = EventAction::FileSystemFileDelete;
    SpinlockGuard guard{lock};

    if(GetUAC().userCanAction(actor, file, PermissionFlag::DELETE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* directory = virtualFilesystem.findContainingDir(file);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    if(!directory->fileDelete(file))
    {
        LOG_EVENT_FILESYS(actor, FAIL_UNSPECIFIED, FilesystemEventData{});
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::fileWrite(UserID actor, FileID file, WriteFileTransaction& transaction)
{
    constexpr EventAction action = EventAction::FilesystemFileWrite;
    SpinlockGuard guard{lock};

    if(GetUAC().userCanAction(actor, file, PermissionFlag::WRITE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualFile* virtualFile = virtualFilesystem.findFile(file);
    if(virtualFile == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_FILE;
    }

    transaction.init(*dataStore, virtualFilesystem);
    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::fileRead(UserID actor, FileID file, size_t begin, size_t end,
                                        ReadFileTransaction& action)
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

StorageStatus StorageEndpoint::dirCreate(UserID actor, FileID dir, const DirectoryCreationInfo& info)
{
    constexpr EventAction action = EventAction::FilesystemDirCreate;
    if(!IsValidFilename(info.name))
    {
        LOG_EVENT_FILESYS(actor, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_INVALID_FILE_NAME;
    }

    if(GetUAC().userCanAction(actor, dir, PermissionFlag::CREATE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* directory = virtualFilesystem.findDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    FileID newId{};
    if(!directory->dirAdd(info, newId))
    {
        LOG_EVENT_FILESYS(actor, FAIL_INVALID_ARGUMENTS, FilesystemEventData{});
        return StorageStatus::ERR_NO_UNIQUE_NAME;
    }

    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::dirDelete(UserID actor, FileID dir)
{
    constexpr EventAction action = EventAction::FileSystemDirDelete;
    SpinlockGuard guard{lock};

    if(GetUAC().userCanAction(actor, dir, PermissionFlag::DELETE) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualDirectory* directory = virtualFilesystem.findContainingDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    if(!directory->dirDelete(dir))
    {
        LOG_EVENT_FILESYS(actor, FAIL_UNSPECIFIED, FilesystemEventData{});
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::dirGetEntries(UserID actor, FileID dir,
                                             std::vector<DTO::ResponseDirectoryEntry>& entries)
{
    constexpr EventAction action = EventAction::FilesystemDirLookup;
    SpinlockGuard guard{lock};
    if(GetUAC().userCanAction(actor, dir, PermissionFlag::READ) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    const VirtualDirectory* directory = virtualFilesystem.findDir(dir);
    if(directory == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_DIR;
    }

    directory->collectEntries(entries);
    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus StorageEndpoint::infoFile(UserID actor, FileID file, DTO::ResponseDirectoryEntry& info)
{
    constexpr EventAction action = EventAction::FilesystemFileInfo;
    SpinlockGuard guard{lock};

    if(GetUAC().userCanAction(actor, file, PermissionFlag::READ) != UACStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_UAC, FilesystemEventData{});
        return StorageStatus::ERR_NO_UAC_PERM;
    }

    VirtualFile* virtualFile = virtualFilesystem.findFile(file);
    if(virtualFile == nullptr)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_FILE, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_FILE;
    }

    info = DTO::ResponseDirectoryEntry::FromFile(*virtualFile);
    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
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
