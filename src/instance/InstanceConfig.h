#ifndef TPUNTK_INSTANCE_CONFIG_H
#define TPUNTK_INSTANCE_CONFIG_H

#include <cstdint>
#include "util/Macros.h"

namespace tpunkt
{

    enum class ConfigParam : uint8_t
    {
        // User

        USER_MAX_ALLOWED_SESSIONS,


        ENUM_SIZE
    };

    struct InstanceConfig final
    {

    private:
        ConfigParam params[ConfigParam::ENUM_SIZE]{};
        TPUNKT_MACROS_STRUCT(InstanceConfig);
    };

    InstanceConfig& GetInstanceConfig();

} // namespace tpunkt

#endif //TPUNTK_INSTANCE_CONFIG_H