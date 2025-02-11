// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_WEBSERVER_H
#define TPUNKT_WEBSERVER_H

#include <uWebSocket/src/App.h>
#include "server/StaticFileStorage.h"

namespace tpunkt
{
    struct WebServer final
    {
        uWS::SSLApp server;
        StaticFileStorage staticFiles;

        WebServer();
        ~WebServer();

        void run();
        void stop();

    private:
    };

    WebServer& GetWebServer();

} // namespace tpunkt

#endif //TPUNKT_WEBSERVER_H