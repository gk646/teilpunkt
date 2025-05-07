// SPDX-License-Identifier: GPL-3.0-only

#include "monitoring/EventMonitor.h"
#include "util/Logging.h"
#include "util/Macros.h"

namespace tpunkt
{

namespace global
{
static EventMonitor* EventMonitor;
}

EventMonitor::EventMonitor()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(EventMonitor);
}

EventMonitor::~EventMonitor()
{
    TPUNKT_MACROS_GLOBAL_RESET(EventMonitor);
}

EventMonitor& GetEventMonitor()
{
    TPUNKT_MACROS_GLOBAL_GET(EventMonitor);
}

template <>
void EventMonitor::logAuditTrace<EventType::Filesystem, FilesystemEventData>(UserID actor, EventAction action,
                                                                             EventStatus status,
                                                                             FilesystemEventData data)
{
}

template <>
void EventMonitor::logAuditTrace<EventType::APIRequest, APIRequestEventData>(UserID actor, EventAction action,
                                                                             EventStatus status,
                                                                             APIRequestEventData data)
{
}

template <>
void EventMonitor::logAuditTrace<EventType::Users, AuthenticationEventData>(UserID actor, EventAction action,
                                                                            EventStatus status,
                                                                            AuthenticationEventData data)
{
}

template <>
void EventMonitor::logAuditTrace<EventType::Server, ServerEventData>(UserID actor, EventAction action,
                                                                     EventStatus status, ServerEventData data)
{
}


} // namespace tpunkt