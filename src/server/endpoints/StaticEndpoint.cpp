#include "server/Endpoints.h"
#include "util/Logger.h"
#include "server/WebServer.h"

namespace tpunkt
{

    void StaticEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
    {
        for (auto [key, value] : *req)
        {
            std::cout << key << ":" << value << std::endl;
            //LOG_INFO("Header:%s | Key:%s", key.data(), value.data());
        }

        const auto& server = GetWebServer();

        auto* request = const_cast<char*>(req->getUrl().data());
        request[req->getUrl().size()] = '\0';

        if (strcmp(request, "/") == 0)
        {
            request = "/index.html";
        }
        const auto file = server.staticFiles.getFile(request);
        request = const_cast<char*>(req->getUrl().data());
        request[req->getUrl().size()] = '\r';
        if (file != nullptr)
        {
            res->end(file->content);
        }
        else
        {
            res->writeStatus("404");
            res->end("404 File not found");
        }
    }
} // namespace tpunkt