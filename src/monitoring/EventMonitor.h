#ifndef TPUNKT_EVENTMONITOR_H
#define TPUNKT_EVENTMONITOR_H


#include "monitoring/Events.h"

namespace tpunkt
{


    struct EventMonitor final
    {


        void log(EventType type, EventAction action, EventStatus status)
        {
        }
    };


   inline EventMonitor& GetEventMonitor()
    {
        EventMonitor hey{};
        return hey;
    }

} // namespace tpunkt


#endif // TPUNKT_EVENTMONITOR_H