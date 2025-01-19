#ifndef TPUNKT_EVENT_MONITOR_H
#define TPUNKT_EVENT_MONITOR_H

#include "monitoring/Events.h"

namespace tpunkt
{
    struct EventMonitor final
    {
        EventMonitor();
        ~EventMonitor();

        void log(EventType type, EventAction action, EventStatus status);
    };

    EventMonitor& GetEventMonitor();

} // namespace tpunkt


#endif // TPUNKT_EVENT_MONITOR_H