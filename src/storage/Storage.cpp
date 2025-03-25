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

Storage& GetStorage()
{
    TPUNKT_MACROS_GLOBAL_GET(Storage);
}

StorageStatus Storage::endpointCreate(const UserID user, const StorageEndpointCreateInfo& info)
{
    SpinlockGuard lock{storageLock};
    const auto adminOnly = GetInstanceConfig().getBool(BoolParamKey::STORAGE_ONLY_ADMIN_CREATE_ENDPOINT);
    if(adminOnly && GetAuthenticator().getIsAdmin(user) != AuthStatus::OK)
    {
        LOG_EVENT(UserAction, EndpointCreate, FAIL_NO_ADMIN);
        return StorageStatus::ERR_NO_ADMIN;
    }

    const auto eid = EndpointID{endpointID++};
    if(StorageEndpoint::CreateDirs(eid))
    {
        endpoints.emplace_front(info, eid, user);
    }
    else
    {
        LOG_EVENT(UserAction, EndpointCreate, FAIL_SERVER_OPERATION);
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, EndpointCreate, SUCCESS);
    return StorageStatus::OK;
}

StorageStatus Storage::endpointCreateFrom(const UserID user, CreateInfo info, const char* file, bool recurse)
{
    if(GetAuthenticator().getIsAdmin(user) != AuthStatus::OK)
    {
        LOG_EVENT(UserAction, EndpointCreateFrom, FAIL_NO_ADMIN);
        return StorageStatus::ERR_NO_ADMIN;
    }

    // Locks internally
    const auto status = endpointCreate(user, info);
    if(status != StorageStatus::OK)
    {
        // Even already logged
        return StorageStatus::ERR_UNSUCCESSFUL;
    }

    SpinlockGuard lock{storageLock};

    // TODO
    LOG_EVENT(UserAction, EndpointCreateFrom, SUCCESS);
    return StorageStatus::OK;
}

StorageStatus Storage::endpointGet(UserID user, const EndpointID endpointId, StorageEndpoint*& ept)
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
        LOG_EVENT(UserAction, EndpointGet, FAIL_NO_SUCH_ENDPOINT);
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    ept = endpointPtr;
    LOG_EVENT(UserAction, EndpointGet, SUCCESS);
    return StorageStatus::OK;
}

StorageStatus Storage::endpointDelete(UserID user, const EndpointID endpoint)
{
    SpinlockGuard lock{storageLock};
    const auto res =
        endpoints.remove_if([ & ](const StorageEndpoint& ept) { return ept.getInfo().eid == endpoint; }) > 0;
    if(!res)
    {
        LOG_EVENT(UserAction, EndpointDelete, FAIL_NO_SUCH_ENDPOINT);
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    LOG_EVENT(UserAction, EndpointDelete, SUCCESS);
    return StorageStatus::OK;
}

uint32_t Storage::getNextID()
{
    SpinlockGuard lock{fileIDLock};
    return filesID++;
}

} // namespace tpunkt