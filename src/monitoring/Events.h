// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_EVENTS_H
#define TPUNKT_EVENTS_H

#include <cstdint>

namespace tpunkt
{

enum class EventType : uint8_t
{
    INVALID,
    APIRequest, // API requests
    Users,      // Register, Login and user information retrieval
    Filesystem, // File-related operations
    Sessions,   // Authentication & session handling
    Instance,   // Instance events like settings, config
    Server,     // Server process events like work tasks
    Host        // Host machine events like resource usage
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
    StorageEndpointCreate,
    StorageEndpointCreateFrom,
    StorageEndpointGet,
    StorageEndpointDelete,
    // Virtual File System
    FileSystemCreateFile,
    FilesystemWriteFile,
    FilesystemRemoveFile,
    FilesystemAddDirectory,
    // TaskManager
    ThreadAdd,
    ThreadRemove,
};

enum class EventStatus : uint8_t
{
    INVALID,
    // Info
    INFO,                   //----------------
    INFO_SUCCESS,           // Success
    INFO_FAIL_UNSPECIFIED,  // Something failed but It's further not specified
    FAIL_INVALID_CREDENTIALS,
    FAIL_INVALID_TOKEN,
    FAIL_CONFIG_RESTRICTED, // Prevented due to instance config
    FAIL_NO_ADMIN,          // Not complete due to missing admin
    FAIL_NO_UAC,            // Denied cause of UAC
    FAIL_USERNAME_EXISTS,
    FAIL_NO_SUCH_ENDPOINT,
    FAIL_NO_SUCH_FILE,
    FAIL_SESSION_EXISTS,
    FAIL_INVALID_ARGUMENTS, // Failed cause arguments had wrong values
    FAIL_INVALID_OPERATION, // Illogic operation or impossible
    FAIL_TOO_MANY,
    WARNING,                //----------------
    // Warnings
    WARN_FAILED_UNSPECIFIED, // Internal operation failed
    ERROR,
    ERROR_OPERATION_FAILED,  // Internal operation failed
};

constexpr bool IsWarnEvent(const EventStatus status)
{
    return status >= EventStatus::WARNING;
}

constexpr bool IsInfoEvent(const EventStatus status)
{
    return status >= EventStatus::INFO && status < EventStatus::WARNING;
}

constexpr bool IsErrorEvent(const EventStatus status)
{
    return status >= EventStatus ::ERROR;
}

} // namespace tpunkt

#endif // TPUNKT_EVENTS_H
