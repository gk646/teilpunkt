#ifndef TESTCOMMONS_H
#define TESTCOMMONS_H

#include <csignal>
#include <sodium/core.h>
#include "util/Logging.h"
#include "auth/Authenticator.h"

static void handleSignal(int signal)
{
}

#define TEST_INIT()                                                                                                    \
    sodium_init();                                                                                                     \
    tpunkt::Logger logger{};                                                                                           \
    tpunkt::Authenticator auth{};                                                                                      \
    tpunkt::CryptoManager crypto{};                                                                                      \
    LOG_INFO("Initialized Testing Environment");                                                                       \
    signal(SIGTRAP, handleSignal);                                                                                     \

#endif // TESTCOMMONS_H