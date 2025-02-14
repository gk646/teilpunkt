// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_TASKMANAGER_H
#define TPUNKT_TASKMANAGER_H

#include <vector>
#include "datastructures/FixedString.h"

namespace tpunkt
{
using TaskName = FixedString<25>;

struct TaskData final
{
};

struct Task
{

    
    virtual bool invoke();

    // Returns true if the task should be removed
    virtual bool shouldBeRemoved() = 0;


  private:
    TaskName name;
    TaskData data;

    TPUNKT_MACROS_STRUCT(Task);
};

struct PeriodicTask final : Task
{
    uint32_t intervalMillis{};
    uint32_t counterMillis{};
    TPUNKT_MACROS_STRUCT(PeriodicTask);
};

struct LimitedTask final : Task
{
    uint32_t executions = 0;
    TPUNKT_MACROS_STRUCT(LimitedTask);
};

struct TaskManager final
{

    bool taskAdd();

    bool taskRemove();

  private:
    std::vector<Task*> tasks;
    TPUNKT_MACROS_STRUCT(TaskManager);
};


TaskManager& GetTaskManager();
} // namespace tpunkt

#endif // TPUNKT_TASKMANAGER_H