// SPDX-License-Identifier: GPL-3.0-only

#include <csignal>
#include <sodium/core.h>
#include "auth/Authenticator.h"
#include "instance/InstanceConfig.h"
#include "instance/MainThread.h"
#include "instance/TaskManager.h"
#include "monitoring/EventLimiter.h"
#include "server/WebServer.h"
#include "storage/Storage.h"
#include "uac/UserAccessControl.h"
#include "util/Logging.h"

namespace
{

void handle_shutdown_signal(const int signal)
{
    if(signal != SIGTRAP)
    {
        tpunkt::GetMainThread().stop();
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
    int status{};
    {
        tpunkt::Logger logger{};
        {
            tpunkt::EventMonitor monitor{};
            tpunkt::InstanceConfig config{};
            tpunkt::CryptoContext crypto{};
            return 0;
            tpunkt::TaskManager tasks{};

            tpunkt::Authenticator auth{};
            tpunkt::EventLimiter limiter{};
            tpunkt::UserAccessControl uac{};
            tpunkt::Storage storage{};
            tpunkt::WebServer webServer{2};

            tpunkt::MainThread main{};
            status = main.run();
        }
    }
    return status;
}