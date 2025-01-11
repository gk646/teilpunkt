#ifndef TPUNKT_WEBSERVER_H
#define TPUNKT_WEBSERVER_H

#include <uWebSocket/src/App.h>
#include "server/StaticFileStorage.h"

namespace tpunkt
{

    struct WebServer final
    {
        uWS::SSLApp server;
        StaticFileStorage staticFiles{TPUNKT_SERVER_STATIC_FILES_DIR};

        WebServer();

        void run();
        void stop();
    };

    WebServer& GetWebServer();

} // namespace tpunkt

#endif //TPUNKT_WEBSERVER_H