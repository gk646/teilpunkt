#ifndef TPUNKT_EVENTS_H
#define TPUNKT_EVENTS_H

#include <cstdint>

namespace tpunkt
{
    enum class EventType : uint8_t
    {
        INVALID,
        API_Request,
        UserAction,
        ServerAction,
        InternalCode,
    };

    enum class EventAction : uint8_t
    {
        INVALID,
        // User
        UserAdd,
        UserLogin,
        UserRemove,
        UserChangeCredentials,
        // Session
        SessionAdd,
        SessionRemove,
        SessionAuthenticate,
        SessionGetSessions,
        // Token
        TokenInvalidate,
        // UserData
        UserDataGetName,
        UserDataGetWrappedKey,
    };

    enum class EventStatus : uint8_t
    {
        INVALID,
        // Info
        INFO,
        Invalid_Authentication,
        Invalid_Token,
        Successful,
        Username_Already_Exists,
        Feature_Disabled,
        WARNINGS,
        // Warnings
        Generic_Unsuccessful,
        SessionMetaData_Mismatch,

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