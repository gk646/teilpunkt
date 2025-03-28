// SPDX-License-Identifier: GPL-3.0-only

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
    SessionGetInfo,
    // Token
    TokenInvalidate,
    // Endpoint
    EndpointCreate,
    EndpointCreateFrom,
    EndpointGet,
    EndpointDelete,
    // Virtual File System
    FilesystemCreateFile,
    FilesystemWriteFile,
    FilesystemRemoveFile,
    FilesystemAddDirectory,
};

enum class EventStatus : uint8_t
{
    INVALID,
    // Info
    INFO,                    //----------------
    SUCCESS,                 // Success
    FAIL_SERVER_UNSPECIFIED, // A server operation failed but It's further not specified
    FAIL_INVALID_CREDENTIALS,
    FAIL_INVALID_TOKEN,
    FAIL_CONFIG_RESTRICTED,  // Prevented due to instance config
    FAIL_NO_ADMIN,           // Not complete due to missing admin
    FAIL_NO_UAC,             // Denied cause of UAC
    FAIL_USERNAME_EXISTS,
    FAIL_NO_SUCH_ENDPOINT,
    FAIL_NO_SUCH_FILE,
    FAIL_SESSION_EXISTS,
    FAIL_INVALID_ARGUMENTS,  // Failed cause arguments had wrong values
    WARNINGS,                //----------------
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