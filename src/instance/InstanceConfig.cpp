#include <cstdlib>
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

    const char* InstanceConfig::getString(const StringParamKey key) const
    {
        CHECK_INVALID_KEY(StringParamKey, key, "");
        return stringParams[ static_cast<int>(key) ].string.c_str();
    }

    uint32_t InstanceConfig::getNumber(const NumberParamKey key) const
    {
        CHECK_INVALID_KEY(NumberParamKey, key, 0);
        return numberParams[ static_cast<int>(key) ].number;
    }

    bool InstanceConfig::getBool(const BoolParamKey key) const
    {
        CHECK_INVALID_KEY(BoolParamKey, key, false);
        return boolParams[ static_cast<int>(key) ].boolean;
    }

    void InstanceConfig::setString(StringParamKey key, const ConfigString& string)
    {
        CHECK_INVALID_KEY(StringParamKey, key,);
        stringParams[ static_cast<int>(key) ].string = string;
    }

    void InstanceConfig::setNumber(NumberParamKey key, const uint32_t number)
    {
        CHECK_INVALID_KEY(NumberParamKey, key,);
        numberParams[ static_cast<int>(key) ].number = number;
    }

    void InstanceConfig::setBool(BoolParamKey key, const bool boolean)
    {
        CHECK_INVALID_KEY(BoolParamKey, key,);
        boolParams[ static_cast<int>(key) ].boolean = boolean;
    }

} // namespace tpunkt