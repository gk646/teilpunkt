#include "server/WebServer.h"
#include "server/Endpoints.h"
#include "util/Logging.h"

namespace tpunkt
{
    namespace global
    {
        static WebServer* Server;
    }

    WebServer::WebServer()
        : server( { "../key/key.pem", "../key/cert.pem", "1234" } ), staticFiles( TPUNKT_SERVER_STATIC_FILES_DIR )
    {
        server.get( "/*", StaticEndpoint::handle );
        server.post( "/api/signup", SignupEndpoint::handle );
        server.post( "/api/login", LoginEndpoint::handle );
        server.post( "/api/upload", UploadEndpoint::handle );
        TPUNKT_MACROS_GLOBAL_ASSIGN( Server );
    }

    WebServer::~WebServer()
    {
        TPUNKT_MACROS_GLOBAL_RESET( Server );
    }

    WebServer& GetWebServer()
    {
        TPUNKT_MACROS_GLOBAL_GET( Server );
    }

    void WebServer::run()
    {
        server.listen( TPUNKT_SERVER_PORT,
                       []( us_listen_socket_t* socket )
                       {
                           if( socket )
                           {
                               LOG_INFO( "Server running at https://localhost:%d", TPUNKT_SERVER_PORT );
                           }
                           else
                           {
                               LOG_ERROR( "Server failed to list" );
                           }
                       } );
        server.run();
    }

    void WebServer::stop()
    {
        LOG_INFO( "Server stop requested" );
        server.close();
    }

} // namespace tpunkt