// SPDX-License-Identifier: Apache License 2.0

#include "auth/Authenticator.h"
#include "auth/AuthToken.h"
#include "instance/InstanceConfig.h"
#include "storage/Storage.h"

namespace tpunkt
{

StorageStatus Storage::endpointCreate(const UserID user, CreateInfo info)
{
    SpinlockGuard lock{storageLock};
    const auto adminOnly = GetInstanceConfig().getBool(BoolParamKey::STORAGE_ONLY_ADMIN_CREATE_ENDPOINT);
    if(adminOnly && GetAuthenticator().getIsAdmin(user) != AuthStatus::OK)
    {
        LOG_EVENT(UserAction, EndpointCreate, FAIL_NO_ADMIN);
        return StorageStatus::ERR_NO_ADMIN;
    }

    // I like this option - its simple and doesn't need move - and cleanup is handled in destructor
    bool success = true;
    endpoints.emplace_back(info, getEndpointID(false), user, success);
    if(!success)
    {
        endpoints.pop_back();
        LOG_EVENT(UserAction, EndpointCreate, WARN_OPERATION_FAILED);
        return StorageStatus::ERR_UNSUCCESSFUL;
    }
    (void)getEndpointID(true);

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

StorageStatus Storage::endpointGet(UserID user, const EndpointID endpointId, StorageEndpoint*& endpoint)
{
    SpinlockGuard lock{storageLock};
    for(auto& ept : endpoints)
    {
        if(ept.getID() == endpointId)
        {
            endpoint = &ept;
            LOG_EVENT(UserAction, EndpointGet, SUCCESS);
            return StorageStatus::OK;
        }
    }

    LOG_EVENT(UserAction, EndpointGet, FAIL_NOT_FOUND);
    return StorageStatus::ERR_NO_SUCH_ENDPOINT;
}

StorageStatus Storage::endpointDelete(UserID user, const EndpointID endpointId)
{
    SpinlockGuard lock{storageLock};
    for(auto& ept : endpoints)
    {
        if(ept.getID() == endpointId)
        {
            ept = std::move(endpoints.back());
            LOG_EVENT(UserAction, EndpointDelete, SUCCESS);
            return StorageStatus::OK;
        }
    }

    LOG_EVENT(UserAction, EndpointDelete, FAIL_NOT_FOUND);
    return StorageStatus::ERR_NO_SUCH_ENDPOINT;
}

FileID Storage::getNextFile(const bool isDirectory, const EndpointID endPoint)
{
    SpinlockGuard lock{storageLock};
    if(fileID >= (UINT32_MAX - 100))
    {
        LOG_WARNING("Getting close to file limit");
    }
    else if(fileID == UINT32_MAX)
    {
        LOG_CRITICAL("Cannot create new file");
    }
    return FileID{.file = fileID++, .endpoint = endPoint, .isDirectory = isDirectory};
}

EndpointID Storage::getEndpointID(const bool increment)
{
    if(increment)
    {
        return EndpointID{endpoint++};
    }
    if(endpoint >= (UINT8_MAX - 10))
    {
        LOG_WARNING("Getting close to endpoint limit");
    }
    else if(endpoint == UINT8_MAX)
    {
        LOG_CRITICAL("Cannot create new endpoint");
    }
    return EndpointID{endpoint};
}

} // namespace tpunkt