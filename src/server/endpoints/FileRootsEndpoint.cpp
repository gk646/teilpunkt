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

void FileRootsEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    // Within a single thread this method is threadsafe
    thread_local std::vector<DTODirectoryInfo> collector;
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
            if(IsRequestTooLarge(res, data, last))
            {
                return;
            }

            // Get roots
            const auto status = Storage::GetInstance().getRoots(user, collector);
            if(status != StorageStatus::OK)
            {
                EndRequest(res, 400, GetStorageStatusStr(status));
                return;
            }

            auto error = glz::write_json(collector, jsonBuffer);
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
