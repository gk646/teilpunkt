#include <cstdlib>
#include "monitoring/EventMonitor.h"
#include "util/Macros.h"
#include "util/Logging.h"

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

    void EventMonitor::log(EventType type, EventAction action, EventStatus status)
    {
        (void)type;
        (void)action;
        (void)status;
    }

} // namespace tpunkt