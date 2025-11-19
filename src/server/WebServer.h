// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_WEBSERVER_H
#define TPUNKT_WEBSERVER_H

#include <App.h>
#include "server/StaticFileStorage.h"

namespace tpunkt
{

struct WebThread final
{
    std::thread thread;
    uWS::SSLApp* app;
    int threadNum = 0;
};

// TODO add stats
struct WebServer final
{
    explicit WebServer(int threads);
    TPUNKT_MACROS_MOVE_ONLY(WebServer);
    ~WebServer();

    [[nodiscard]] const StaticFileStorage& getStaticFileStorage();

  private:
    uWS::SSLApp& getNextHandler();

    StaticFileStorage staticFiles;
    std::vector<WebThread> webThreads;
    int roundRobinNum = 0;
    int threadCount = 0;

    friend uWS::SSLApp& GetNextHandler();
    friend void WebThreadFunc(int, uWS::SocketContextOptions*, volatile bool*);
};

WebServer& GetWebServer();

} // namespace tpunkt

#endif // TPUNKT_WEBSERVER_H
