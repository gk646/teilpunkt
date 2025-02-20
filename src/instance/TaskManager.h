// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_TASKMANAGER_H
#define TPUNKT_TASKMANAGER_H

#include <atomic>
#include <thread>
#include <vector>
#include "datastructures/FixedString.h"
#include "instance/Task.h"

namespace tpunkt
{

// Simple tasks are taken by any thread and executed
struct TaskManager final
{
    explicit TaskManager(uint32_t threadCount);
    ~TaskManager();

    //===== Task Management =====//

    template <typename Callable>
    TaskID taskAdd(const TaskName& name, Callable&& func)
    {
        // TODO make slot memory allocator - reuse from magique
        auto* task = new SimpleTask(std::forward<Callable>(func));
        return implTaskAdd(name, *task);
    }

    template <typename Callable>
    TaskID taskAddPeriodic(const TaskName& name, Callable&& func, uint32_t intervalMicros, uint32_t times)
    {
        auto task = new PeriodicTask(intervalMicros, times, std::forward<Callable>(func));
        return implTaskAdd(name, *task);
    }

    bool taskRemove(TaskID task);

    //===== Thread Management =====//

    // Returns true if new thread was added
    bool threadAdd();

    // Returns true if any thread was removed
    bool threadRemove();

    // Thread info

  private:
    // Takes ownership
    TaskID implTaskAdd(Task& task);

    struct TaskData final
    {
        Task* task;
        TaskName name;
    };

    struct ThreadStats final
    {
        uint64_t tasksExecutedTotal;
        uint64_t tasksPerSec;
    };

    struct ThreadData final
    {
        ThreadStats stats;
        std::thread thread;
    };

    std::vector<TaskData> tasks;
    std::vector<ThreadData> threads;
    uint64_t taskID;
    std::atomic<bool> stop;
    TPUNKT_MACROS_STRUCT(TaskManager);
};

TaskManager& GetTaskManager();

} // namespace tpunkt

#endif // TPUNKT_TASKMANAGER_H