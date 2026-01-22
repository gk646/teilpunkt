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
    size_t threadNum = 0;
};

// TODO add stats
struct WebServer final
{
    explicit WebServer(size_t threads);
    TPUNKT_MACROS_DEL_CTORS(WebServer);
    ~WebServer();

    [[nodiscard]] const StaticFileStorage& getStaticFileStorage();

  private:
    uWS::SSLApp& getNextHandler();

    StaticFileStorage staticFiles;
    std::vector<WebThread> webThreads;
    size_t roundRobinNum = 0;
    size_t threadCount = 0;

    friend uWS::SSLApp& GetNextHandler();
    friend void WebThreadFunc(size_t, const uWS::SocketContextOptions*, volatile bool*);
};

WebServer& GetWebServer();

} // namespace tpunkt

#endif // TPUNKT_WEBSERVER_H
