// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_EVENTS_H
#define TPUNKT_EVENTS_H

#include <cstdint>

namespace tpunkt
{

enum class EventType : uint8_t
{
    INVALID,
    API_Request,       // User-initiated API requests
    UserAction,        // Actions performed by a user
    FileSystem,        // File-related operations
    SessionManagement, // Authentication & session handling
    Security,          // Security-related events
    ServerAction,      // Server events (non-user initiated)
};

enum class EventAction : uint8_t
{
    INVALID,
    // User
    UserAdd,
    UserLogin,
    UserRemove,
    UserChangeCredentials,
    // UserData
    UserDataGetName,
    UserDataGetWrappedKey,
    // Session
    SessionAdd,
    SessionRemove,
    SessionAuthenticate,
    SessionGetSessions,
    // Token
    TokenInvalidate,
    // Storage
    StorageCreateEndpoint,
    StorageCreateEndpointFrom,
    StorageGetEndpoint,
    StorageDeleteEndpoint,
    // Virtual File System
    FilesystemAddFile,
    FilesystemRemoveFile,
    FilesystemAddDirectory,
    FilesystemAdd
};

enum class EventStatus : uint8_t
{
    INVALID,
    // Info
    INFO,                   //----------------
    SUCCESS,                // Generic success
    FAIL_INVALID_CREDENTIALS,
    FAIL_INVALID_TOKEN,
    FAIL_CONFIG_RESTRICTED, // Prevented due to instance config
    FAIL_NO_ADMIN,          // Not complete due to missing admin
    FAIL_USERNAME_EXISTS,
    FAIL_NOT_FOUND,
    WARNINGS,               //----------------
    // Warnings
    WARN_OPERATION_FAILED, // Internal operation failed
};

constexpr bool IsWarnEvent(const EventStatus status)
{
    return status >= EventStatus::WARNINGS;
}

constexpr bool IsInfoEvent(const EventStatus status)
{
    return status >= EventStatus::INFO && status < EventStatus::WARNINGS;
}

} // namespace tpunkt

#endif // TPUNKT_EVENTS_H