// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include "server/DTO.h"
#include "server/DTOMappings.h"
#include "server/Endpoints.h"
#include "storage/Storage.h"

namespace tpunkt
{

void DirCreateEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    // Within a single thread this method is threadsafe
    thread_local std::vector<DTO::DirectoryEntry> collector;
    thread_local std::string jsonBuffer(TPUNKT_SERVER_JSON_THREAD_BUFFER_START, '0');

    if(!IsRateLimited(res, req))
    {
        return;
    }

    UserID user{};
    if(!HasValidSession(res, req, user))
    {
        return;
    }

    res->onData(
        [ res, user ](std::string_view data, const bool last)
        {
            if(!last) // Too long
            {
                EndRequest(res, 431, "Sent data too large");
                return;
            }

            DTO::DirectoryRequest infoRequest;
            auto error = glz::read_json(infoRequest, data);
            if(error)
            {
                EndRequest(res, 400, "Bad sent JSON");
                return;
            }

            StorageEndpoint* endpoint = nullptr;
            auto status = Storage::GetInstance().endpointGet(user, {}, endpoint);
            if(status != StorageStatus::OK)
            {
                EndRequest(res, 400, GetStorageStatusStr(status));
                return;
            }

            status = endpoint->dirGetInfo(user, infoRequest.directory, collector);
            if(status != StorageStatus::OK)
            {
                EndRequest(res, 400, GetStorageStatusStr(status));
                return;
            }

            error = glz::write_json(collector, jsonBuffer);
            if(error)
            {
                EndRequest(res, 500, "Bad generated JSON");
                return;
            }

            EndRequest(res, 200, jsonBuffer.c_str());
        });


    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt
