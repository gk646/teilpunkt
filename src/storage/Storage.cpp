// SPDX-License-Identifier: GPL-3.0-only

#include "auth/Authenticator.h"
#include "auth/AuthToken.h"
#include "instance/InstanceConfig.h"
#include "storage/Storage.h"

namespace tpunkt
{

namespace
{

StorageEndpoint* GetEndpoint(const EndpointID eid, BlockList<StorageEndpoint>& endpoints)
{
    for(auto& endpoint : endpoints)
    {
        if(endpoint.getIdx() == static_cast<uint32_t>(eid))
        {
            return &endpoint.get();
        }
    }
    return nullptr;
}

} // namespace

StorageStatus Storage::endpointCreate(const UserID user, const StorageEndpointCreateInfo& info)
{
    SpinlockGuard lock{storageLock};
    const auto adminOnly = GetInstanceConfig().getBool(BoolParamKey::STORAGE_ONLY_ADMIN_CREATE_ENDPOINT);
    if(adminOnly && GetAuthenticator().getIsAdmin(user) != AuthStatus::OK)
    {
        LOG_EVENT(UserAction, EndpointCreate, FAIL_NO_ADMIN);
        return StorageStatus::ERR_NO_ADMIN;
    }

    uint32_t idx = 0;
    endpoints.add(idx, info, EndpointID{idx}, user);

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
    auto* endpoint = GetEndpoint(endpointId, endpoints);
    if(endpoint == nullptr) [[unlikely]]
    {
        LOG_EVENT(UserAction, EndpointGet, FAIL_NO_SUCH_ENDPOINT);
        return StorageStatus::ERR_NO_SUCH_ENDPOINT;
    }
    ept = endpoint;
    LOG_EVENT(UserAction, EndpointGet, SUCCESS);
    return StorageStatus::OK;
}

StorageStatus Storage::endpointDelete(UserID user, const EndpointID endpointId)
{
    SpinlockGuard lock{storageLock};
    for(auto& ept : endpoints)
    {
        if(ept.getIdx() == static_cast<uint32_t>(endpointId) && ept.get().canBeRemoved())
        {
            (void)endpoints.remove(static_cast<uint32_t>(endpointId));
            LOG_EVENT(UserAction, EndpointDelete, SUCCESS);
            return StorageStatus::OK;
        }
    }

    LOG_EVENT(UserAction, EndpointDelete, FAIL_NO_SUCH_ENDPOINT);
    return StorageStatus::ERR_NO_SUCH_ENDPOINT;
}

// TODO use block storage

/*
FileID getNextFile(const bool isDirectory, const EndpointID endPoint)
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

*/


template <typename T>
BlockStorage<T>& BlockList<T>::GetStore()
{
    if constexpr(std::is_same_v<T, VirtualFile>)
    {
        return GetStorage().getFileStore();
    }
    else if constexpr(std::is_same_v<T, VirtualDirectory>)
    {
        return GetStorage().getDirStore();
    } else if constexpr(std::is_same_v<T, StorageEndpoint>)
    {
        return GetStorage().getEndpointStore();
    }
    else
    {
        static_assert(std::is_same_v<int, VirtualFile>, "Wrong type");
    }
}


template BlockStorage<VirtualFile>& BlockList<VirtualFile>::GetStore();
template BlockStorage<VirtualDirectory>& BlockList<VirtualDirectory>::GetStore();

} // namespace tpunkt