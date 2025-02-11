// SPDX-License-Identifier: Apache License 2.0

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

    void log(EventType type, EventAction action, EventStatus status);

    template <EventType type, typename EventData>
    void logData(EventAction action, EventStatus status, EventData data);
};


EventMonitor& GetEventMonitor();

} // namespace tpunkt


#endif // TPUNKT_EVENT_MONITOR_H