#include <csignal>
#include <sodium/core.h>
#include "server/WebServer.h"
#include "util/Logger.h"

static void handle_shutdown_signal(int /**/) { tpunkt::GetWebServer().stop(); }

int main()
{
    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);
    tpunkt::GetLogger().init();
    if (sodium_init() != 0)
    {
        LOG_ERROR("Failed to initialize sodium");
        return 1;
    }
    tpunkt::GetWebServer().run();
    tpunkt::GetLogger().shutdown();
    return 0;
}