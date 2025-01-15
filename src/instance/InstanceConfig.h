#ifndef TPUNKT_INSTANCE_CONFIG_H
#define TPUNKT_INSTANCE_CONFIG_H

#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{
// TODO rework into 3 different types for the tree param types with their own enum checked at compile time with type check
    // All config parameters including their default value
    enum class ConfigKey : uint8_t
    {
        // Max amount of session per user
        // 3
        USER_MAX_ALLOWED_SESSIONS,
        // Max amount of tasks (e.g. uploads, downloads etc.) a user can have at the same time
        // 3
        USER_MAX_ALLOWED_TASKS,
        // Cooldown before any given ip address can create another account
        // 30
        USER_IP_ACCOUNT_CREATION_TIMEOUT_MIN,
        // If set only the instance admin can create new accounts
        // false
        USER_ONLY_ADMIN_CREATE_ACCOUNT,
        // Name of this instance
        // teilpunkt-instance
        INSTANCE_NAME,
        // Max allowed number of requests to the server per user per minute
        // 50
        API_REQUESTS_PER_USER_PER_MIN,
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
            FixedString<TPUNKT_INSTANCE_CONFIG_STRING_LEN> string;
            int64_t integer{};
            bool boolean;
        };
        friend InstanceConfig;
    };

    struct InstanceConfig final
    {
        InstanceConfig();
        ~InstanceConfig();
        [[nodiscard]] const ConfigParam& getParam(ConfigKey key) const;

        void setParam(ConfigKey key);

      private:
        ConfigParam params[ static_cast<int>(ConfigKey::ENUM_SIZE) ]{};
        TPUNKT_MACROS_STRUCT(InstanceConfig);
    };

    InstanceConfig& GetInstanceConfig();

} // namespace tpunkt

#endif // TPUNKT_INSTANCE_CONFIG_H