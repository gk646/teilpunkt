// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_TASKMANAGER_H
#define TPUNKT_TASKMANAGER_H

#include <atomic>
#include <thread>
#include <vector>
#include "datastructures/FixedString.h"
#include "instance/Task.h"
#include "server/DTO.h"

namespace tpunkt
{

struct TaskManager final
{
    explicit TaskManager();
    ~TaskManager();

    //===== Task Management =====//

    template <typename Callable>
    TaskID taskAdd(const UserID actor, const TaskName& name, Callable&& func)
    {
        // TODO make slot memory allocator - reuse from magique
        auto* task = new OneTimeTask(std::forward<Callable>(func));
        return implTaskAdd(actor, name, *task);
    }

    // Returns true if the task was found and removed - only works if task was added but not yet started
    bool taskRemove(TaskID task);

    //===== Thread Management =====//

    // Returns true if new thread was added - requires admin
    bool threadAdd(UserID actor);

    // Returns true if any thread was removed - requires admin
    bool threadRemove(UserID actor);

    //===== Info =====//

    bool infoTasks(UserID actor, std::vector<DTOTaskInfo>& collector);

    bool infoThreads(UserID actor, std::vector<DTOThreadInfo>& collector);

    bool infoManager(UserID actor, DTOTaskManagerInfo& info);

  private:
    // Takes ownership
    TaskID implTaskAdd(UserID actor, const TaskName& name, Task& task);

    using ThreadID = uint32_t;
    static void workerTask(ThreadID tid, TaskManager* manager);

    //===== Thread =====//

    struct ThreadStatus final
    {
        Timestamp start;
        std::atomic<uint64_t> tasksExecutedTotal = 0; // Total count completed
        std::atomic<uint64_t> totalTaskTimeNanos = 0; // Total time spent on tasks in nanoseconds
        std::atomic<bool> isWorking{false};           // Currently processing a task
        std::atomic<bool> isEmployed{true};           // If the thread takes on new tasks
    };

    struct ThreadData final
    {
        ThreadID tid;                                 // identifier
        volatile ThreadStatus* status = nullptr;      // Assigned by the thread
        std::thread thread;                           // thread handle
    };

    //===== Task =====//

    struct TaskStatus final
    {
        ThreadID worker = 0;       // The current worker
        Timestamp start;           // Start of processing
        Timestamp done;            // Start of processing
        bool isProcessing = false; // Currently processed
        bool isDone = false;
    };

    struct TaskData final
    {
        TaskStatus status;
        Task* task = nullptr;
        TaskName name;
        TaskID taskId = TaskID{0};
        Timestamp added;
        UserID autor;
    };

    std::vector<TaskData> tasks;
    std::vector<ThreadData> threads;
    Spinlock taskLock;
    Spinlock threadLock;

    std::atomic<bool> isRunning{true};
    uint32_t taskID = 1;
    uint32_t threadID = 1;
    TPUNKT_MACROS_STRUCT(TaskManager);
};

TaskManager& GetTaskManager();

} // namespace tpunkt

#endif // TPUNKT_TASKMANAGER_H