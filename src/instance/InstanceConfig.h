#ifndef TPUNKT_INSTANCE_CONFIG_H
#define TPUNKT_INSTANCE_CONFIG_H

#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{
    // check All config parameters including their default value

    enum class StringParamKey : uint8_t
    {
        INVALID,
        // Name of this instance
        // teilpunkt-instance
        INSTANCE_NAME,
        ENUM_SIZE
    };

    enum class NumberParamKey : uint8_t
    {
        INVALID,
        // Cooldown before any given ip address can create another account
        // 30
        USER_IP_ACCOUNT_CREATION_TIMEOUT_MIN,
        // Max amount of session per user
        // 3
        USER_MAX_ALLOWED_SESSIONS,
        // Max amount of tasks (e.g. uploads, downloads etc.) a user can have at the same time
        // 3
        USER_MAX_ALLOWED_TASKS,
        // Time until a new session expires in seconds
        // 1 day = 24 * 60 * 60
        USER_SESSION_EXPIRATION_DELAY_SECS,
        // Max allowed number of requests to the server per user per minute
        // 50
        API_REQUESTS_PER_USER_PER_MIN,
        ENUM_SIZE
    };

    enum class BoolParamKey : uint8_t
    {
        INVALID,
        // If set only the instance admin can create new accounts - account creation is disabled for non-admins
        // Also disables name side channel attacks (get registered names by querying registered ones)
        // false
        USER_ONLY_ADMIN_CREATE_ACCOUNT,
        ENUM_SIZE
    };

    struct StringConfigParam final
    {
        ConfigString string{};
    };

    struct NumberConfigParam final
    {
        int32_t number;
    };

    struct BoolConfigParam final
    {
        bool boolean;
    };

    // Type safe storage for different config parameters - avoids template as its only little code to write
    struct InstanceConfig final
    {
        InstanceConfig();
        ~InstanceConfig();

        // Returns the config string for the given value
        // Will NEVER be nullptr
        [[nodiscard]] const char* getString(StringParamKey key) const;
        [[nodiscard]] uint32_t getNumber(NumberParamKey key) const;
        [[nodiscard]] bool getBool(BoolParamKey key) const;

        void setString(StringParamKey key, const ConfigString& string);
        void setNumber(NumberParamKey key, uint32_t number);
        void setBool(BoolParamKey key, bool boolean);

      private:
        void setupDefaults();
        StringConfigParam stringParams[ static_cast<int>(StringParamKey::ENUM_SIZE) ]{};
        NumberConfigParam numberParams[ static_cast<int>(NumberParamKey::ENUM_SIZE) ]{};
        BoolConfigParam boolParams[ static_cast<int>(BoolParamKey::ENUM_SIZE) ]{};
        TPUNKT_MACROS_STRUCT(InstanceConfig);
    };

    InstanceConfig& GetInstanceConfig();

} // namespace tpunkt

#endif // TPUNKT_INSTANCE_CONFIG_H