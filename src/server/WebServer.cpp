#include <csignal>
#include "server/WebServer.h"
#include "server/Endpoints.h"
#include "util/Logger.h"

namespace tpunkt
{


    WebServer::WebServer() : server({"../key/key.pem", "../key/cert.pem", "1234"})
    {

        server.get("/*", StaticEndpoint::handle);
        server.post("/api/signup", SignupEndpoint::handle);
        server.post("/api/login", LoginEndpoint::handle);
    }

    void WebServer::run()
    {
        server.listen(TPUNKT_SERVER_PORT,
                      [](us_listen_socket_t* socket)
                      {
                          if (socket)
                          {
                              LOG_INFO("Server running at https://localhost:%d", TPUNKT_SERVER_PORT);
                          }
                      });
        server.run();
    }

    void WebServer::stop()
    {
        LOG_INFO("Server stopping");
        server.close();
    }

    static WebServer SERVER{};

    WebServer& GetWebServer() { return SERVER; }

} // namespace tpunkt