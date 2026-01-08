// SPDX-License-Identifier: GPL-3.0-only
#include <HttpResponse.h>
#include "server/Endpoints.h"
#include "storage/Storage.h"

namespace tpunkt
{

void FileCreateEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(!IsRateLimited(res, req))
    {
        return;
    }
    UserID user{};
    if(!HasValidSession(res, req, user))
    {
        return;
    }

    const auto handlerFunc = [ user ](std::string_view data, const bool last)
    {
        auto& storage = Storage::GetInstance();

        StorageEndpoint* endpoint = nullptr;
        auto status = storage.endpointGet(user, {}, endpoint);
        if(status != StorageStatus::OK)
        {

        }
    };

    res->onData(handlerFunc);
    res->onAborted([ res ] { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt
