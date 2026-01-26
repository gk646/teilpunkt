// SPDX-License-Identifier: GPL-3.0-only
#include <HttpResponse.h>
#include "server/Endpoints.h"
#include "storage/Storage.h"
#include "server/DTOMappings.h"

namespace tpunkt
{

void FileCreateEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(IsRateLimited(res, req))
    {
        return;
    }
    UserID user{};
    if(!HasValidSession(res, req, user))
    {
        return;
    }

    const auto handlerFunc = [ user, res ](std::string_view data, const bool isLast)
    {
        if(!isLast) // Too long
        {
            EndRequest(res, 431, "Sent data too large");
            return;
        }

        DTO::RequestCreateFile request;
        auto error = glz::read_json(request, data);
        if(error)
        {
            EndRequest(res, 400, "Sent bad JSON");
            return;
        }

        StorageEndpoint* endpoint = nullptr;
        auto status = Storage::GetInstance().endpointGet(user, request.directory.getEndpoint(), endpoint);
        if(status != StorageStatus::OK)
        {
            EndRequest(res, 400, GetStorageStatusStr(status));
            return;
        }

        const FileCreationInfo info{.name = request.name, .creator = user, .endpoint = request.directory.getEndpoint()};
        status = endpoint->fileCreate(user, request.directory, info);
        if(status != StorageStatus::OK)
        {
            EndRequest(res, 400, GetStorageStatusStr(status));
            return;
        }

        EndRequest(res, 200);
    };

    res->onData(handlerFunc);
    res->onAborted([ res ] { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt
