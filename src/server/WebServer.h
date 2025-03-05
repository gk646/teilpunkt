// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_WEBSERVER_H
#define TPUNKT_WEBSERVER_H

#include <App.h>
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