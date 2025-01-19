#include <csignal>
#include <sodium/core.h>
#include "util/Logger.h"
#include "server/WebServer.h"
#include "auth/Authenticator.h"
#include "instance/InstanceConfig.h"

static void handle_shutdown_signal(int /**/)
{
    tpunkt::GetWebServer().stop();
}

int main()
{
    {
        TPUNKT_MACROS_STARTUP_PRINT();
    }
    if(sodium_init() != 0)
    {
        fputs("Failed to initialize libsodium", stderr);
        return 1;
    }
    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);

    // All variables are declared on the stack and cleaned up in a fixed scope
    {
        tpunkt::Logger logger{};
        LOG_EVENT(UserAction, UserLogin, Invalid_Authentication);
        LOG_EVENT(UserAction, UserLogin, Successful);
        return 0;
        {
            tpunkt::InstanceConfig config{};
            tpunkt::Authenticator auth{};
            tpunkt::WebServer server{};
            server.run();
        }
    }
    return 0;
}