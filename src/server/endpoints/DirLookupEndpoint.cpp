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

void DirLookupEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    // Within a single thread this method is threadsafe
    thread_local std::vector<DTO::ResponseDirectoryEntry> collector;
    thread_local std::string jsonBuffer(TPUNKT_SERVER_JSON_THREAD_BUFFER_START, '0');

   TPUNKT_MACROS_AUTH_USER()

    collector.clear();
    jsonBuffer.clear();

    res->onData(
        [ res, user ](std::string_view data, const bool isLast)
        {
            if(!isLast) // Too long
            {
                EndRequest(res, 431, "Sent data too large");
                return;
            }

            DTO::RequestDirectoryInfo request;
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

            status = endpoint->dirGetEntries(user, request.directory, collector);
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
