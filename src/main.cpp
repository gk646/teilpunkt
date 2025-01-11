#include <sodium/core.h>
#include "server/WebServer.h"
#include "util/Logger.h"

int main()
{
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