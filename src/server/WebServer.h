#ifndef TPUNKT_WEBSERVER_H
#define TPUNKT_WEBSERVER_H

namespace tpunkt
{
    struct WebServer final
    {
        void run();
    };
    WebServer& GetWebServer();

} // namespace tpunkt

#endif //TPUNKT_WEBSERVER_H