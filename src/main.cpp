#include <sodium.h>
#include <spdlog/spdlog.h>

#include "server/WebServer.h"
#include "logging/Logger.h"

int main()
{
    tpunkt::GetLogger().init();
    if (sodium_init() != 0)
    {
        spdlog::error("Failed to initialize sodium");
        return 1;
    }
    tpunkt::GetWebServer().run();
    spdlog::shutdown();
    return 0;
}