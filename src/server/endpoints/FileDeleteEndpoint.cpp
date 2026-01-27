// SPDX-License-Identifier: GPL-3.0-only
#include <HttpResponse.h>
#include "server/Endpoints.h"
#include "storage/Storage.h"
#include "server/DTOMappings.h"

namespace tpunkt
{

void FileDeleteEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    TPUNKT_MACROS_AUTH_USER()

    const auto handlerFunc = [ user, res ](std::string_view data, const bool isLast)
    {
        if(!isLast) // Too long
        {
            EndRequest(res, 431, "Sent data too large");
        }

        DTO::RequestDelete request;
        auto error = glz::read_json(request, data);
        if(error)
        {
            EndRequest(res, 400, "Sent bad JSON");
            return;
        }

        StorageEndpoint* endpoint = nullptr;
        auto status = Storage::GetInstance().endpointGet(user, request.file.getEndpoint(), endpoint);
        if(status != StorageStatus::OK)
        {
            EndRequest(res, 400, GetStorageStatusStr(status));
            return;
        }

        status = endpoint->fileDelete(user, request.file);
        if(status != StorageStatus::OK)
        {
            EndRequest(res, 400, GetStorageStatusStr(status));
            return;
        }

        EndRequest(res, 200, "OK");
    };

    res->onData(handlerFunc);
    res->onAborted([ res ] { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt
