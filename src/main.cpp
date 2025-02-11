// SPDX-License-Identifier: Apache License 2.0

#include <csignal>
#include <sodium/core.h>
#include "util/Logging.h"
#include "server/WebServer.h"
#include "auth/Authenticator.h"
#include "instance/InstanceConfig.h"

namespace
{
    void handle_shutdown_signal(const int signal)
    {
        if(signal != SIGTRAP)
        {
            tpunkt::GetWebServer().stop();
        }
    }
} // namespace


int32_t main()
{
    {
        TPUNKT_MACROS_STARTUP_PRINT();
    }

    (void)signal(SIGINT, handle_shutdown_signal);
    (void)signal(SIGTERM, handle_shutdown_signal);
    (void)signal(SIGTRAP, handle_shutdown_signal);

    if(sodium_init() != 0)
    {
        (void)fputs("Failed to initialize libsodium", stderr);
        return 1;
    }
    // All variables are declared on the stack and cleaned up in a fixed scope
    {
        tpunkt::Logger logger{};
        {
            tpunkt::CryptoManager crypto{};
            tpunkt::InstanceConfig config{};
            tpunkt::Authenticator auth{};
            tpunkt::EventMonitor monitor{};

            tpunkt::WebServer server{};
            server.run();
        }
    }
    return 0;
}