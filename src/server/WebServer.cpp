#include <spdlog/spdlog.h>
#include "server/WebServer.h"

namespace tpunkt
{

    static constexpr const char* STATIC_FILES_FOLDER = "../static";
    static constexpr int  SERVER_PORT = 8080;

    WebServer::WebServer()
    {
        server.set_mount_point("/", STATIC_FILES_FOLDER);
        // server.set_pre_routing_handler();
        server.set_logger(
            [](const httplib::Request& req, const httplib::Response& res)
            {
                auto isDynamic = req.path.find("/api/") == 0; // Adjust prefix as needed
                if (isDynamic)
                {
                    spdlog::info(
                "Request Info:\n"
                "  IP: {}\n"
                "  Method: {}\n"
                "  Path: {}\n"
                "  Headers: {}\n"
                "Response Info:\n"
                "  Status: {}\n"
                "  Body: {}",
                req.remote_addr,                       // Client IP
                req.method,                            // HTTP method (GET, POST, etc.)
                req.path,                              // Request path
                [&req]() {                             // Headers as a string
                    std::string headers;
                    for (const auto &[key, value] : req.headers) {
                        headers += key + ": " + value + ", ";
                    }
                    return headers.empty() ? "None" : headers;
                }(),
                res.status,                            // HTTP response status
                res.body                               // Response body
            );
                }
            });
    }

    void WebServer::run()
    {
        spdlog::info("Server running at http://localhost:{}",SERVER_PORT);
        server.listen("0.0.0.0", SERVER_PORT);
    }

    void WebServer::stop()
    {
        spdlog::info("Server stopping");
        server.stop();
    }

    static WebServer SERVER;
    WebServer& GetWebServer() { return SERVER; }

} // namespace tpunkt