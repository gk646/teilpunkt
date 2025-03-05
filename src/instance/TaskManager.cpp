// SPDX-License-Identifier: GPL-3.0-only

#include <thread>
#include "instance/TaskManager.h"

namespace tpunkt
{

namespace global
{
TaskManager* TaskManager;
}

namespace
{

void taskThread(const std::atomic<bool>& isRunning)
{
    while(isRunning)
    {

    }
}

} // namespace


TaskManager::TaskManager(const uint32_t threadCount)
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(TaskManager);
    for(uint32_t i = 0; i < threadCount; ++i)
    {
        //std::thread thread{taskThread};
        //threads.emplace_back();
    }
}

TaskManager::~TaskManager()
{
    TPUNKT_MACROS_GLOBAL_RESET(TaskManager);

}

TaskManager& GetTaskManager()
{
    TPUNKT_MACROS_GLOBAL_GET(TaskManager);
}

bool TaskManager::taskRemove(TaskID task)
{
}

TaskID TaskManager::implTaskAdd(Task& task)
{
}

} // namespace tpunkt