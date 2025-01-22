#ifndef TESTCOMMONS_H
#define TESTCOMMONS_H

#include <csignal>
#include <sodium/core.h>
#include "util/Logging.h"
#include "auth/Authenticator.h"
#include "instance/InstanceConfig.h"

static void handleSignal(int signal){}

#define TEST_INIT()                                                                                                    \
    (void)sodium_init();                                                                                               \
    tpunkt::Logger logger{};                                                                                           \
    tpunkt::CryptoManager crypto{};                                                                                    \
    tpunkt::InstanceConfig config{};                                                                                   \
    tpunkt::Authenticator auth{};                                                                                      \
    tpunkt::EventMonitor monitor{};                                                                                    \
    LOG_INFO("Initialized Testing Environment");                                                                       \
    signal(SIGTRAP, handleSignal);

#endif // TESTCOMMONS_H