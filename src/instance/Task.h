// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_TASK_H
#define TPUNKT_TASK_H

#include "datastructures/Timestamp.h"

namespace tpunkt
{

enum class TaskType : uint8_t
{
    ONE_TIME_TASK,
    PERIODIC_TASK,
};

struct Task
{
    explicit Task(const TaskType type) : type(type)
    {
    }

    virtual ~Task() = default;

    // Task has no return value we care about - handles errors in its own context
    virtual void invoke() = 0;

    // Returns true if the task should be removed
    virtual bool shouldBeRemoved() = 0;

    // Checked before task is invoked
    virtual bool shouldBeInvoked() = 0;

  protected:
    TaskType type;
};

template <typename Callable>
struct PeriodicTask final : Task
{
    explicit PeriodicTask(Callable&& func, const uint32_t intervalMicros, const uint32_t limit)
        : Task(TaskType::PERIODIC_TASK), func(std::move(func)), nextExecution(Timestamp::Now()),
          intervalMicros(intervalMicros), countLimit(limit)
    {
    }

    void invoke() override
    {
        func();
        count++;
        nextExecution.addMicros(intervalMicros);
    }

    bool shouldBeInvoked() override
    {
        return nextExecution.isInPast();
    }

    bool shouldBeRemoved() override
    {
        return count >= countLimit;
    }

  private:
    Callable func;
    Timestamp nextExecution;
    uint32_t intervalMicros{};
    uint32_t countLimit = 0;
    uint32_t count = 0;
    TPUNKT_MACROS_STRUCT(PeriodicTask);
};

template <typename Callable>
struct OneTimeTask final : Task
{
    explicit OneTimeTask(Callable&& func) : Task(TaskType::ONE_TIME_TASK), func(std::move(func))
    {
    }

    void invoke() override
    {
        func();
        executed = true;
    }

    bool shouldBeInvoked() override
    {
        return !executed;
    }

    bool shouldBeRemoved() override
    {
        return executed;
    }

  private:
    Callable func;
    bool executed = false;
    TPUNKT_MACROS_STRUCT(OneTimeTask);
};

} // namespace tpunkt

#endif // TPUNKT_TASK_H