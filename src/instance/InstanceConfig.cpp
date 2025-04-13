// SPDX-License-Identifier: GPL-3.0-only

#include "instance/InstanceConfig.h"

namespace tpunkt
{
namespace global
{
static InstanceConfig* InstanceConfig;
}

InstanceConfig::InstanceConfig()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(InstanceConfig);
    for(uint8_t i = 1; i < (uint8_t)StringParamKey::ENUM_SIZE; ++i)
    {
        const auto key = StringParamKey{i};
        setString(key, ConfigString(getDefault(key)));
    }
    for(uint8_t i = 1; i < (uint8_t)NumberParamKey::ENUM_SIZE; ++i)
    {
        const auto key = NumberParamKey{i};
        setNumber(key, getDefault(key));
    }
    for(uint8_t i = 1; i < (uint8_t)BoolParamKey::ENUM_SIZE; ++i)
    {
        const auto key = BoolParamKey{i};
        setBool(key, getDefault(key));
    }
}

InstanceConfig::~InstanceConfig()
{
    TPUNKT_MACROS_GLOBAL_RESET(InstanceConfig);
}

InstanceConfig& GetInstanceConfig()
{
    TPUNKT_MACROS_GLOBAL_GET(InstanceConfig);
}

#define CHECK_INVALID_KEY(type, key, ret)                                                                              \
    if(key == type::INVALID || key == type::ENUM_SIZE)                                                                 \
    {                                                                                                                  \
        LOG_FATAL("Wrong Key");                                                                                        \
        return ret;                                                                                                    \
    }

const char* InstanceConfig::getString(const StringParamKey key)
{
    SpinlockGuard guard{configLock};
    CHECK_INVALID_KEY(StringParamKey, key, "");
    return stringParams[ static_cast<int>(key) ].string.c_str();
}

uint32_t InstanceConfig::getNumber(const NumberParamKey key)
{
    SpinlockGuard guard{configLock};
    CHECK_INVALID_KEY(NumberParamKey, key, 0);
    return numberParams[ static_cast<int>(key) ].number;
}

bool InstanceConfig::getBool(const BoolParamKey key)
{
    SpinlockGuard guard{configLock};
    CHECK_INVALID_KEY(BoolParamKey, key, false);
    return boolParams[ static_cast<int>(key) ].boolean;
}

void InstanceConfig::setString(StringParamKey key, const ConfigString& string)
{
    SpinlockGuard guard{configLock};
    CHECK_INVALID_KEY(StringParamKey, key, );
    stringParams[ static_cast<int>(key) ].string = string;
}

void InstanceConfig::setNumber(NumberParamKey key, const uint32_t number)
{
    SpinlockGuard guard{configLock};
    CHECK_INVALID_KEY(NumberParamKey, key, );
    numberParams[ static_cast<int>(key) ].number = number;
}

void InstanceConfig::setBool(BoolParamKey key, const bool boolean)
{
    SpinlockGuard guard{configLock};
    CHECK_INVALID_KEY(BoolParamKey, key, );
    boolParams[ static_cast<int>(key) ].boolean = boolean;
}

const char* InstanceConfig::getDefault(const StringParamKey key)
{
    switch(key)
    {
        case StringParamKey::INSTANCE_NAME:
            return "teilpunkt-instance";
        case StringParamKey::INVALID:
        case StringParamKey::ENUM_SIZE:
            break;
    }
    LOG_FATAL("Wrong key");
}

uint32_t InstanceConfig::getDefault(const NumberParamKey key)
{
    switch(key)
    {
        case NumberParamKey::USER_IP_ACCOUNT_CREATION_TIMEOUT_MIN:
            return 30;
        case NumberParamKey::USER_MAX_ALLOWED_TASKS:
            return 2;
        case NumberParamKey::USER_MAX_ALLOWED_SESSIONS:
            return 3;
        case NumberParamKey::USER_SESSION_EXPIRATION_SECS:
            return 24 * 60 * 60; // 1 day in seconds
        case NumberParamKey::API_REQUESTS_PER_USER_PER_MIN:
            return 40;
        case NumberParamKey::STORAGE_MAX_TOTAL_FILES_OR_DIRS:
            return 50'000;
        case NumberParamKey::INSTANCE_WORKER_THREADS:
            return 2;
        case NumberParamKey::INVALID:
        case NumberParamKey::ENUM_SIZE:
            break;
    }
    LOG_FATAL("Wrong key");
}

bool InstanceConfig::getDefault(const BoolParamKey key)
{
    switch(key)
    {
        case BoolParamKey::USER_ONLY_ADMIN_CREATE_ACCOUNT:
            return false;
        case BoolParamKey::STORAGE_ONLY_ADMIN_CREATE_ENDPOINT:
            return true;
        case BoolParamKey::INVALID:
        case BoolParamKey::ENUM_SIZE:
            break;
    }
    LOG_FATAL("Wrong key");
}


} // namespace tpunkt