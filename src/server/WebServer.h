#ifndef TPUNKT_WEBSERVER_H
#define TPUNKT_WEBSERVER_H

#include <httplib/httplib.h>

namespace tpunkt
{
    using HTTPHandlerFunc = std::function<void(const httplib::Request&, httplib::Response&)>;

    struct WebServer final
    {
        httplib::Server server;

        WebServer();

        void run();
        void stop();
    };

    WebServer& GetWebServer();

} // namespace tpunkt

#endif //TPUNKT_WEBSERVER_H