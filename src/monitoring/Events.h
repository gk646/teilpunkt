#ifndef TPUNKT_EVENTS_H
#define TPUNKT_EVENTS_H

namespace tpunkt
{

    enum class EventType : uint8_t
    {
        INVALID,
        API_Request,
        UserAction,
        InternalCode,
    };

    enum class EventAction : uint8_t
    {
        INVALID,
        UserLogin,
    };

    enum class EventStatus : uint8_t
    {
        INVALID,
        // Info
        Invalid_Authentication,
        Successful,

        WARNINGS,
        // Warnings
    };

    constexpr bool IsWarnEvent(const EventStatus status)
    {
        return status >= EventStatus::WARNINGS;
    }

} // namespace tpunkt

#endif // TPUNKT_EVENTS_H