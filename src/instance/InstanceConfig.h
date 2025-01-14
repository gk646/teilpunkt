#ifndef TPUNKT_INSTANCE_CONFIG_H
#define TPUNKT_INSTANCE_CONFIG_H

#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{
    enum class ConfigKey : uint8_t
    {
        // Max amount of session per user
        USER_MAX_ALLOWED_SESSIONS,
        // Max amount of tasks (e.g. uploads, downloads etc.) a user can have at the same time
        USER_MAX_ALLOWED_TASKS,
        // Name of this instance
        INSTANCE_NAME,
        // Amount of params
        ENUM_SIZE
    };

    enum class ConfigParamType : uint8_t
    {
        INVALID,
        STRING,
        NUMBER,
        BOOL
    };

    struct ConfigParam final
    {

        [[nodiscard]] const char* getString() const;

        [[nodiscard]] bool getBool() const;

        [[nodiscard]] int64_t getInteger() const;

      private:
        ConfigParamType type{};
        union
        {
            FixedString<TPUNKT_INSTANCE_CONFIG_STRING_LEN> string{};
            int64_t integer;
            bool boolean;
        };
        friend InstanceConfig;
    };

    struct InstanceConfig final
    {
        InstanceConfig();
        ~InstanceConfig();
        [[nodiscard]] const ConfigParam& getParam( ConfigKey key ) const;

        void setParam( ConfigKey key );

      private:
        ConfigParam params[ static_cast<int>( ConfigKey::ENUM_SIZE ) ]{};
        TPUNKT_MACROS_STRUCT( InstanceConfig );
    };

    InstanceConfig& GetInstanceConfig();

} // namespace tpunkt

#endif // TPUNKT_INSTANCE_CONFIG_H