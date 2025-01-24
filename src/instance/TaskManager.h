#ifndef TPUNKT_TASKMANAGER_H
#define TPUNKT_TASKMANAGER_H

#include <vector>
#include "datastructures/FixedString.h"

namespace tpunkt
{
    using TaskName = FixedString<25>;

    struct Task
    {
        TaskName name;
    };

    struct PeriodicTask final : Task
    {
        uint32_t intervalMillis{};
        uint32_t counterMillis{};
    };

    struct TaskManager final
    {

        std::vector<Task*> tasks;
    };


    TaskManager& GetTaskManager();
} // namespace tpunkt

#endif // TPUNKT_TASKMANAGER_H