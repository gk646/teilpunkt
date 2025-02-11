// SPDX-License-Identifier: Apache License 2.0

#include "auth/Authenticator.h"
#include "auth/AuthToken.h"
#include "instance/InstanceConfig.h"
#include "storage/Storage.h"

namespace tpunkt
{

StorageStatus Storage::endpointCreate(const AuthToken& token, CreateInfo info)
{
    SpinlockGuard lock{storageLock};
    TPUNKT_NO_AUTH_RET_ADD(token, StorageStatus::ERR_NO_AUTH, LOG_EVENT(UserAction, StorageCreateEndpoint, SUCCESS));

    if(GetInstanceConfig().getBool(BoolParamKey::STORAGE_ONLY_ADMIN_CREATE_ENDPOINT))
    {
        if(GetAuthenticator().getIsAdmin(token, token.getUser()) != AuthStatus::OK)
        {
            LOG_EVENT(UserAction, StorageCreateEndpoint, FAIL_NO_ADMIN);
            return StorageStatus::ERR_NO_ADMIN;
        }
    }

    // TODO creation needs error handling
    bool success = true;
    endpoints.emplace_back(info, getNextEndpoint(), token.getUser(), success);
    if(!success)
    {
    }


    LOG_EVENT(UserAction, StorageCreateEndpoint, SUCCESS);
    return StorageStatus::OK;
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
    return FileID{.fileID = fileID++, .endpoint = endPoint, .isDirectory = isDirectory};
}

EndpointID Storage::getNextEndpoint()
{
    if(endpoint >= (UINT8_MAX - 10))
    {
        LOG_WARNING("Getting close to endpoint limit");
    }
    else if(endpoint == UINT8_MAX)
    {
        LOG_CRITICAL("Cannot create new endpoint");
    }
    return EndpointID{endpoint++};
}

} // namespace tpunkt