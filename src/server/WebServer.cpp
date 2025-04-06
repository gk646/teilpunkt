// SPDX-License-Identifier: GPL-3.0-only

#include "server/Endpoints.h"
#include "server/WebServer.h"
#include "util/Logging.h"

namespace tpunkt
{

namespace global
{
static WebServer* Server;
}

WebServer::WebServer()
    : server({.key_file_name = "../key/key.pem", .cert_file_name = "../key/cert.pem", .passphrase = "1234"}),
      staticFiles(TPUNKT_SERVER_STATIC_FILES_DIR)
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(Server);
    // Authentication
    server.post("/api/auth/password", AuthPasswordEndpoint::handle);
    //    server.post("/api/auth/passkey-get", AuthPasskeyGetEndpoint::handle);
    // server.post("/api/auth/passkey", AuthPasskeyValidateEndpoint::handle);
    // Register
    server.post("/api/register/password", RegisterPasswordEndpoint::handle);
//    server.get("/api/register/passkey-get", RegisterPasskeyOptionsEndpoint::handle);
    server.post("/api/register/passkey", RegisterPasskeyEndpoint::handle);
    //  Filesystem
    server.post("/api/filesystem/upload", FileUploadEndpoint::handle);
    server.get("/api/filesystem/download", FileDownloadEndpoint::handle);
    server.get("/api/filesystem/directory", FileDirectoryEndpoint::handle);
    server.get("/api/filesystem/roots", FileRootsEndpoint::handle);
    // Misc
    server.get("/*", StaticEndpoint::handle);
}

WebServer::~WebServer()
{
    TPUNKT_MACROS_GLOBAL_RESET(Server);
}

WebServer& GetWebServer()
{
    TPUNKT_MACROS_GLOBAL_GET(Server);
}

void WebServer::run()
{
    server.listen(TPUNKT_SERVER_PORT,
                  [](const us_listen_socket_t* socket)
                  {
                      if(socket)
                      {
                          LOG_INFO("Server running at https://localhost:%d", TPUNKT_SERVER_PORT);
                      }
                      else
                      {
                          LOG_ERROR("Server failed create socket");
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