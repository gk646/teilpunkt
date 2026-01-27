// SPDX-License-Identifier: GPL-3.0-only
#include "auth/Authenticator.h"
#include "instance/InstanceConfig.h"
#include "storage/Storage.h"
#include "util/Wrapper.h"

namespace tpunkt
{

namespace global
{
Storage* Storage;
}

Storage::Storage()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(Storage);
    LOG_INFO("Startup Disk Usage: %2d%%", GetDiskUsage());

    // TODO - properly detect first startup
    CreateInfo info1{.name = "Endpoint 1", .maxSize = 10000, .type = StorageEndpointType::LOCAL_FILE_SYSTEM};
    endpointCreate(UserID::SERVER, info1);

    CreateInfo info2{.name = "Endoint 2", .maxSize = 10000, .type = StorageEndpointType::LOCAL_FILE_SYSTEM};
    endpointCreate(UserID::SERVER, info2);
}

Storage::~Storage()
{
    TPUNKT_MACROS_GLOBAL_RESET(Storage);
}

Storage& Storage::GetInstance()
{
    TPUNKT_MACROS_GLOBAL_GET(Storage);
}

StorageStatus Storage::getRoots(UserID user, std::vector<DTO::ResponseDirectoryInfo>& roots)
{
    roots.clear();

    // TODO fix
    for(StorageEndpoint& endpoint : endpoints)
    {
        roots.push_back(DTO::ResponseDirectoryInfo::FromDir(endpoint.virtualFilesystem.getRoot()));
    }

    return StorageStatus::OK;
}

StorageStatus Storage::endpointCreate(const UserID actor, const StorageEndpointCreateInfo& info)
{
    constexpr auto action = EventAction::StorageEndpointCreate;
    SpinlockGuard lock{storageLock};
    // TODO Use a permission table to reuse roles
    if(Authenticator::GetInstance().getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_ADMIN, FilesystemEventData{});
        return StorageStatus::ERR_NO_ADMIN;
    }

    const auto eid = static_cast<EndpointID>(endpointID++);
    if(StorageEndpoint::CreateDirs(eid))
    {
        endpoints.emplace_front(info, eid, actor);
    }
    else
    {
        LOG_EVENT_FILESYS(actor, FAIL_UNSPECIFIED, FilesystemEventData{});
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus Storage::endpointCreateFrom(const UserID actor, CreateInfo info, const char* file, bool recurse)
{
    constexpr auto action = EventAction::StorageEndpointCreateFrom;
    if(Authenticator::GetInstance().getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_ADMIN, FilesystemEventData{});
        return StorageStatus::ERR_NO_ADMIN;
    }

    // Locks internally
    const auto status = endpointCreate(actor, info);
    if(status != StorageStatus::OK)
    {
        // Even already logged
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    SpinlockGuard lock{storageLock};
    LOG_FATAL("Not implemented");

    // TODO
    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus Storage::endpointGet(UserID actor, const EndpointID endpoint, StorageEndpoint*& ept)
{
    constexpr auto action = EventAction::StorageEndpointGet;
    SpinlockGuard lock{storageLock};
    StorageEndpoint* endpointPtr = nullptr;

    for(auto& savedEp : endpoints)
    {
        if(savedEp.getData().endpoint == endpoint)
        {
            endpointPtr = &savedEp;
            break;
        }
    }
    if(endpointPtr == nullptr) [[unlikely]]
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_ENDPOINT, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    ept = endpointPtr;
    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus Storage::endpointDelete(UserID actor, const EndpointID endpoint)
{
    constexpr auto action = EventAction::StorageEndpointDelete;
    SpinlockGuard lock{storageLock};
    const auto res =
        endpoints.remove_if([ & ](const StorageEndpoint& ept) { return ept.getData().endpoint == endpoint; }) > 0;
    if(!res)
    {
        LOG_EVENT_FILESYS(actor, FAIL_NO_SUCH_ENDPOINT, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    LOG_EVENT_FILESYS(actor, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

} // namespace tpunkt
