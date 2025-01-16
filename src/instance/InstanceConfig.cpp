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

    const char* ConfigParam::getString() const
    {
        if(type != ConfigParamType::STRING)
        {
            LOG_FATAL("Accessing wrong type");
            return nullptr;
        }
        return string.c_str();
    }

    bool ConfigParam::getBool() const
    {
        if(type != ConfigParamType::BOOL)
        {
            LOG_FATAL("Accessing wrong type");
            return false;
        }
        return boolean;
    }

    int64_t ConfigParam::getInteger() const
    {
        if(type != ConfigParamType::NUMBER)
        {
            LOG_FATAL("Accessing wrong type");
            return 0;
        }
        return integer;
    }

} // namespace tpunkt