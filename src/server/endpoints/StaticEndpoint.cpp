// SPDX-License-Identifier: GPL-3.0-only

#include "server/Endpoints.h"
#include "server/WebServer.h"
#include "util/Logging.h"

namespace tpunkt
{
void StaticEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(HandleRequest(res, req))
    {
        return;
    }
    const auto& server = GetWebServer();
    const auto request = req->getUrl().data();

    const StaticFile* file = nullptr;
    if(strcmp(request, "/") == 0)
    {
        file = server.staticFiles.getFile("/index.html");
    }
    else
    {
        file = server.staticFiles.getFile(request);
    }
    if(file != nullptr)
    {
        res->writeHeader("Content-Type", file->type);
        EndRequest(res, 200, file->content);
    }
    else
    {
        EndRequest(res, 404, "Static file not found");
    }
}

} // namespace tpunkt