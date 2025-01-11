#include <uWebSocket/src/App.h>
#include "server/Endpoints.h"
#include "datastructures/STFTable.h"
#include "util/Strings.h"
#include "util/Logger.h"


namespace tpunkt
{
    void SignupEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
    {
        for (auto pair : *req)
        {
            LOG_INFO("Header:%s | Key:%s", pair.first.data(), pair.second.data());
        }

        /*
    TextParser<{"username", 16}, {"password", 32}> stfTable;
    if (!stfTable.parse(req.  body.c_str()))
    {
       // tpunkt::ClearString(req.body);
        return rejectRequest(res, httplib::StatusCode::BadRequest_400);
    }
    */
    }

} // namespace tpunkt