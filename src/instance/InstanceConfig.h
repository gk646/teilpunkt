// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_INSTANCE_CONFIG_H
#define TPUNKT_INSTANCE_CONFIG_H

#include <array>
#include "datastructures/FixedString.h"
#include "datastructures/Spinlock.h"
#include "fwd.h"
#include "util/Macros.h"

namespace tpunkt
{

// All config parameters
// Default values visible in the constructor

enum class StringParamKey : uint8_t
{
    INVALID,
    // Name of this instance
    INSTANCE_NAME,
    ENUM_SIZE
};

enum class NumberParamKey : uint8_t
{
    INVALID,
    // Cooldown before any given ip address can create another account
    USER_IP_ACCOUNT_CREATION_TIMEOUT_MIN,
    // Max amount of session per user
    USER_MAX_ALLOWED_SESSIONS,
    // Max amount of tasks (e.g. uploads, downloads etc.) a user can have at the same time
    USER_MAX_ALLOWED_TASKS,
    // Time until a new session expires in seconds
    USER_SESSION_EXPIRATION_SECS,
    // Max allowed number of requests to the server per user per minute
    API_REQUESTS_PER_USER_PER_MIN,
    // Max amount of files and directories (each) the storage can hold across all endpoints
    STORAGE_MAX_TOTAL_FILES_OR_DIRS,
    // Worker threads
    INSTANCE_WORKER_THREADS,
    ENUM_SIZE
};

enum class BoolParamKey : uint8_t
{
    INVALID,
    // If set only the instance admin can create new accounts - account creation is disabled for non-admins
    // Also disables name side channel attacks (get registered names by trying to create accounts)
    USER_ONLY_ADMIN_CREATE_ACCOUNT,
    // If true only admins can create new endpoints
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
    static const char* GetDefault(StringParamKey key);
    static uint32_t GetDefault(NumberParamKey key);
    static bool GetDefault(BoolParamKey key);

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
    std::array<StringConfigParam, static_cast<int>(StringParamKey::ENUM_SIZE)> stringParams;
    std::array<NumberConfigParam, static_cast<int>(NumberParamKey::ENUM_SIZE)> numberParams;
    std::array<BoolConfigParam, static_cast<int>(BoolParamKey::ENUM_SIZE)> boolParams;
    Spinlock configLock;
    TPUNKT_MACROS_STRUCT(InstanceConfig);
};

InstanceConfig& GetInstanceConfig();

} // namespace tpunkt

#endif // TPUNKT_INSTANCE_CONFIG_H
