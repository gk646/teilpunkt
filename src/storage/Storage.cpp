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
}

Storage::~Storage()
{
    TPUNKT_MACROS_GLOBAL_RESET(Storage);
}

StorageStatus Storage::getRoots(UserID user, std::vector<DTODirectoryInfo>& roots)
{
    for(uint32_t i = 0; i < 10; ++i)
    {
        roots.push_back({"TestFile", {i, EndpointID{i}, true}});
    }
}

Storage& GetStorage()
{
    TPUNKT_MACROS_GLOBAL_GET(Storage);
}

StorageStatus Storage::endpointCreate(const UserID actor, const StorageEndpointCreateInfo& info)
{
    SpinlockGuard lock{storageLock};
    // TODO Use a permission table to reuse roles
    if(GetAuthenticator().getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT(actor, Filesystem, StorageEndpointCreate, FAIL_NO_ADMIN, FilesystemEventData{});
        return StorageStatus::ERR_NO_ADMIN;
    }

    const auto eid = EndpointID{endpointID++};
    if(StorageEndpoint::CreateDirs(eid))
    {
        endpoints.emplace_front(info, eid, actor);
    }
    else
    {
        LOG_EVENT(actor, Filesystem, StorageEndpointCreate, INFO_FAIL_UNSPECIFIED, FilesystemEventData{});
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(actor, Filesystem, StorageEndpointCreate, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus Storage::endpointCreateFrom(const UserID actor, CreateInfo info, const char* file, bool recurse)
{
    if(GetAuthenticator().getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT(actor, Filesystem, StorageEndpointCreateFrom, FAIL_NO_ADMIN, FilesystemEventData{});
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
    LOG_EVENT(actor, Filesystem, StorageEndpointCreateFrom, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus Storage::endpointGet(UserID actor, const EndpointID endpointId, StorageEndpoint*& ept)
{
    SpinlockGuard lock{storageLock};
    StorageEndpoint* endpointPtr = nullptr;
    for(auto& savedEp : endpoints)
    {
        if(savedEp.getInfo().eid == endpointId)
        {
            endpointPtr = &savedEp;
        }
    }
    if(endpointPtr == nullptr) [[unlikely]]
    {
        LOG_EVENT(actor, Filesystem, StorageEndpointGet, FAIL_NO_SUCH_ENDPOINT, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    ept = endpointPtr;
    LOG_EVENT(actor, Filesystem, StorageEndpointGet, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

StorageStatus Storage::endpointDelete(UserID actor, const EndpointID endpoint)
{
    SpinlockGuard lock{storageLock};
    const auto res =
        endpoints.remove_if([ & ](const StorageEndpoint& ept) { return ept.getInfo().eid == endpoint; }) > 0;
    if(!res)
    {
        LOG_EVENT(actor, Filesystem, StorageEndpointDelete, FAIL_NO_SUCH_ENDPOINT, FilesystemEventData{});
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    LOG_EVENT(actor, Filesystem, StorageEndpointDelete, INFO_SUCCESS, FilesystemEventData{});
    return StorageStatus::OK;
}

uint32_t Storage::getNextID()
{
    SpinlockGuard lock{fileIDLock};
    return filesID++;
}

} // namespace tpunkt