// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_EVENT_MONITOR_H
#define TPUNKT_EVENT_MONITOR_H

#include "monitoring/EventData.h"
#include "monitoring/Events.h"

namespace tpunkt
{

struct EventMonitor final
{
    EventMonitor();
    ~EventMonitor();

    template <EventType type, typename EventData>
    void logAuditTrace(UserID actor, EventAction action, EventStatus status, EventData data);
};

template <>
void EventMonitor::logAuditTrace<EventType::Filesystem, FilesystemEventData>(UserID actor, EventAction action,
                                                                             EventStatus status,
                                                                             FilesystemEventData data);
template <>
void EventMonitor::logAuditTrace<EventType::APIRequest, APIRequestEventData>(UserID actor, EventAction action,
                                                                             EventStatus status,
                                                                             APIRequestEventData data);
template <>
void EventMonitor::logAuditTrace<EventType::Users, AuthenticationEventData>(UserID actor, EventAction action,
                                                                                     EventStatus status,
                                                                                     AuthenticationEventData data);
template <>
void EventMonitor::logAuditTrace<EventType::Server, ServerEventData>(UserID actor, EventAction action,
                                                                             EventStatus status,
                                                                             ServerEventData data);


EventMonitor& GetEventMonitor();

} // namespace tpunkt


#endif // TPUNKT_EVENT_MONITOR_H