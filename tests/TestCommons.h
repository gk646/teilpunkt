#ifndef TESTCOMMONS_H
#define TESTCOMMONS_H

#include <csignal>
#include <sodium/core.h>
#include "auth/Authenticator.h"
#include "crypto/CryptoContext.h"
#include "instance/InstanceConfig.h"
#include "util/Logging.h"
#include "storage/Storage.h"
#include "uac/UserAccessControl.h"

static void handleSignal(int signal){}

#define TEST_INIT()                                                                                                    \
    (void)sodium_init();                                                                                               \
    tpunkt::Logger logger{};                                                                                           \
    tpunkt::CryptoContext crypto{};                                                                                    \
    tpunkt::InstanceConfig config{};                                                                                   \
    tpunkt::Authenticator auth{};                                                                                      \
    tpunkt::EventMonitor monitor{};                                                                                    \
    tpunkt::UserAccessControl uac{};                                                                                   \
    tpunkt::Storage storage{};                                                                                         \
    LOG_INFO("Initialized Testing Environment");                                                                       \
    (void)signal(SIGTRAP, handleSignal)

#endif // TESTCOMMONS_H