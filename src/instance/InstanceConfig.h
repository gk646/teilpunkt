// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_INSTANCE_CONFIG_H
#define TPUNKT_INSTANCE_CONFIG_H

#include <cstdint>
#include "datastructures/FixedString.h"
#include "datastructures/Spinlock.h"
#include "fwd.h"
#include "util/Macros.h"

namespace tpunkt
{

// All config parameters including their default value

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
    // 1
    USER_MAX_ALLOWED_TASKS,
    // Time until a new session expires in seconds
    // 1 day = 24 * 60 * 60
    USER_SESSION_EXPIRATION_DELAY_SECS,
    // Max allowed number of requests to the server per user per minute
    // 40
    API_REQUESTS_PER_USER_PER_MIN,
    // Max amount of files and directories (each) the storage can hold across all endpoints
    // 50'000
    STORAGE_MAX_TOTAL_FILES_OR_DIRS,
    ENUM_SIZE
};

enum class BoolParamKey : uint8_t
{
    INVALID,
    // If set only the instance admin can create new accounts - account creation is disabled for non-admins
    // Also disables name side channel attacks (get registered names by trying to create accounts)
    // false
    USER_ONLY_ADMIN_CREATE_ACCOUNT,
    // If true only admins can create new endpoints
    // true
    STORAGE_ONLY_ADMIN_CREATE_ENDPOINT,
    ENUM_SIZE
};


// Type safe storage for different config parameters - avoids templates as its only little code to write
struct InstanceConfig final
{
    InstanceConfig();
    ~InstanceConfig();

    // Returns the config string for the given value
    // Will NEVER be nullptr
    [[nodiscard]] const char* getString(StringParamKey key);
    [[nodiscard]] uint32_t getNumber(NumberParamKey key);
    [[nodiscard]] bool getBool(BoolParamKey key);

    void setString(StringParamKey key, const ConfigString& string);
    void setNumber(NumberParamKey key, uint32_t number);
    void setBool(BoolParamKey key, bool boolean);


  private:
    [[nodiscard]] const char* getDefault(StringParamKey key);
    [[nodiscard]] uint32_t getDefault(NumberParamKey key);
    [[nodiscard]] bool getDefault(BoolParamKey key);

    struct StringConfigParam final
    {
        ConfigString string;
    };
    struct NumberConfigParam final
    {
        uint32_t number;
    };
    struct BoolConfigParam final
    {
        bool boolean;
    };
    StringConfigParam stringParams[ static_cast<int>(StringParamKey::ENUM_SIZE) ]{};
    NumberConfigParam numberParams[ static_cast<int>(NumberParamKey::ENUM_SIZE) ]{};
    BoolConfigParam boolParams[ static_cast<int>(BoolParamKey::ENUM_SIZE) ]{};
    Spinlock configLock;
    TPUNKT_MACROS_STRUCT(InstanceConfig);
};

InstanceConfig& GetInstanceConfig();

} // namespace tpunkt

#endif // TPUNKT_INSTANCE_CONFIG_H