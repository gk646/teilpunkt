#ifndef TPUNKT_EVENT_MONITOR_H
#define TPUNKT_EVENT_MONITOR_H

#include "monitoring/Events.h"
#include "monitoring/EventData.h"

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