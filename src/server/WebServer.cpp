#include "server/WebServer.h"
#include "server/Endpoints.h"
#include "util/Logger.h"

namespace tpunkt
{
    static WebServer* SERVER;

    WebServer::WebServer() :
        server({"../key/key.pem", "../key/cert.pem", "1234"}), staticFiles(TPUNKT_SERVER_STATIC_FILES_DIR)
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN(SERVER);
        server.get("/*", StaticEndpoint::handle);
        server.post("/api/signup", SignupEndpoint::handle);
        server.post("/api/login", LoginEndpoint::handle);
    }

    WebServer::~WebServer()
    {
        TPUNKT_MACROS_GLOBAL_RESET(SERVER);
        LOG_INFO("Server stopped");
    }

    WebServer& GetWebServer() { TPUNKT_MACROS_GLOBAL_GET(SERVER); }

    void WebServer::run()
    {
        server.listen(TPUNKT_SERVER_PORT,
                      [](us_listen_socket_t* socket)
                      {
                          if (socket)
                          {
                              LOG_INFO("Server running at https://localhost:%d", TPUNKT_SERVER_PORT);
                          }
                          else
                          {
                              LOG_ERROR("Server failed to list");
                          }
                      });
        server.run();
    }

    void WebServer::stop()
    {
        LOG_INFO("Server stop requested");
        server.close();
    }

} // namespace tpunkt