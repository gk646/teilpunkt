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

void EventMonitor::log(const EventType type, const EventAction action, const EventStatus status)
{
    (void)type;
    (void)action;
    (void)status;
}

template <EventType type, typename EventData>
void EventMonitor::logData(EventAction action, EventStatus status, EventData data)
{
}

template <>
void EventMonitor::logData<EventType::FileSystem, FileSystemEventData>(EventAction action, EventStatus status,
                                                                       FileSystemEventData data)
{
}


} // namespace tpunkt