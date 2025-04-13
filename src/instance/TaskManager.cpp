// SPDX-License-Identifier: GPL-3.0-only

#include <cfloat>
#include <mutex>
#include "instance/InstanceConfig.h"
#include "instance/TaskManager.h"
#include "util/Logging.h"

namespace tpunkt
{

namespace global
{
TaskManager* TaskManager;
}

TaskManager::TaskManager()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(TaskManager);
    ThreadData data;
    const uint32_t threadCount = GetInstanceConfig().getNumber(NumberParamKey::INSTANCE_WORKER_THREADS);
    for(uint32_t i = 0; i < threadCount; ++i)
    {
        threadAdd(UserID::SERVER);
    }
}

TaskManager& GetTaskManager()
{
    TPUNKT_MACROS_GLOBAL_GET(TaskManager);
}

TaskManager::~TaskManager()
{
    auto getUnfinishedTasks = [ & ]
    {
        uint32_t unfinishedTasks = 0;
        SpinlockGuard guard{taskLock};
        for(const auto& task : tasks)
        {
            if(!task.status.isDone)
            {
                ++unfinishedTasks;
            }
        }
        return unfinishedTasks;
    };

    const uint32_t unfinishedTasks = getUnfinishedTasks();
    if(unfinishedTasks > 0)
    {
        LOG_WARNING("Waiting for unfinished tasks: %u", unfinishedTasks);
        while(getUnfinishedTasks() != 0u)
        {
            usleep(1000);
        }
    }
    isRunning = false;
    for(auto& [ tid, status, thread ] : threads)
    {
        if(thread.joinable())
        {
            thread.join();
        }
    }
    TPUNKT_MACROS_GLOBAL_RESET(TaskManager);
}

void TaskManager::workerTask(const ThreadID tid, TaskManager* manager)
{
    volatile ThreadStatus status;

    // Assign status
    for(auto& val : manager->threads)
    {
        if(val.tid == tid)
        {
            val.status = &status;
            break;
        }
    }

    auto getTask = [ & ] -> TaskData*
    {
        SpinlockGuard guard{manager->taskLock};
        for(auto& task : manager->tasks)
        {
            if(!task.status.isProcessing || task.status.isDone)
            {
                task.status.isProcessing = true;
                task.status.worker = tid;
                task.status.start = Timestamp::Now();
                return &task;
            }
        }
        return nullptr;
    };

    auto removeTask = [ & ](TaskData& task)
    {
        SpinlockGuard guard{manager->taskLock};
        task.status.isProcessing = false;
        task.status.isDone = true;
        task.status.done = Timestamp::Now();
    };

    while(manager->isRunning && status.isEmployed)
    {
        TaskData* taskData = getTask();
        if(taskData != nullptr)
        {
            status.isWorking = true;
            if(taskData->task->shouldBeInvoked())
            {
                auto start = Timestamp::Now();
                taskData->status.start = start;

                taskData->task->invoke();

                auto diff = Timestamp::Now() - start;
                ++status.tasksExecutedTotal;
                status.totalTaskTimeNanos += diff.getNanos();
            }
            if(taskData->task->shouldBeRemoved())
            {
                removeTask(*taskData);
            }
            status.isWorking = false;
        }
        usleep(1000U);
    }
}

bool TaskManager::threadAdd(const UserID actor)
{
    SpinlockGuard guard{threadLock};
    const auto maxThreads = std::thread::hardware_concurrency();
    if(threads.size() >= maxThreads - 1)
    {
        LOG_EVENT(actor, Server, ThreadAdd, FAIL_TOO_MANY, ServerEventData{});
        return false;
    }

    auto& [ tid, status, thread ] = threads.emplace_back(threadID++);
    thread = std::thread(workerTask, tid, this);
    LOG_EVENT(actor, Server, ThreadAdd, INFO_SUCCESS, ServerEventData{});
    return true;
}

bool TaskManager::threadRemove(const UserID actor)
{
    SpinlockGuard guard{threadLock};
    if(threads.size() == 1)
    {
        LOG_EVENT(actor, Server, ThreadRemove, FAIL_INVALID_OPERATION, ServerEventData{});
        return false;
    }

    auto& [ tid, status, thread ] = threads.back();
    status->isEmployed = false;
    if(thread.joinable())
    {
        thread.join();
        threads.pop_back();
        threadID--;
        LOG_EVENT(actor, Server, ThreadRemove, INFO_SUCCESS, ServerEventData{});
        return true;
    }
    LOG_EVENT(actor, Server, ThreadRemove, ERROR_OPERATION_FAILED, ServerEventData{});
    return false;
}

bool TaskManager::infoTasks(UserID actor, std::vector<DTOTaskInfo>& collector)
{
    // TODO add logging
    SpinlockGuard guard{taskLock};
    for(auto& val : tasks)
    {
        DTOTaskInfo info;
        info.name = val.name;
        info.addedUnixNanos = val.added.getNanos();

        if(val.status.isProcessing)
        {
            info.startUnixNanos = val.status.start.getNanos();
        }

        if(val.status.isDone)
        {
            info.doneUnixNanos = val.status.done.getNanos();
            info.startUnixNanos = val.status.start.getNanos();
        }

        info.threadID = val.status.worker;
        info.taskID = val.taskId;
        collector.push_back(info);
    }
    return true;
}

bool TaskManager::taskRemove(const TaskID task)
{
    // TODO add admin check and actor (if actor is not autor)
    SpinlockGuard guard{taskLock};
    for(auto& val : tasks)
    {
        if(val.taskId == task)
        {
            val = tasks.back();
            tasks.pop_back();
            return true;
        }
    }
    return false;
}

TaskID TaskManager::implTaskAdd(const UserID actor, const TaskName& name, Task& task)
{
    SpinlockGuard guard{taskLock};
    TaskData data{};
    data.status.done.zero();
    data.status.start.zero();

    data.task = &task;
    data.name = name;
    data.taskId = TaskID{taskID++};
    data.added = Timestamp::Now();
    data.autor = actor;

    tasks.push_back(data);
    return data.taskId;
}


} // namespace tpunkt