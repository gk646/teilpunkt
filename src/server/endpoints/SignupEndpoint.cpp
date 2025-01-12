#include <uWebSocket/src/App.h>
#include "server/Endpoints.h"
#include "datastructures/STFTable.h"
#include "util/Strings.h"
#include "util/Logger.h"

namespace tpunkt
{
    void SignupEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
    {
        for (auto [key, value] : *req)
        {
            std::cout << key << ":" << value << std::endl;
            //LOG_INFO("Header:%s | Key:%s", key.data(), value.data());
        }

        res->onData([res](std::string_view data, bool last) {
            std::cout << "Got data: " << data.length() << ", it's the last: " << last << std::endl;

            if (last) {
                res->end("Thanks for the data!");
            }
            res->end();
        });

        res->onAborted([]()
        {

        });

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