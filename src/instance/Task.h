// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_TASK_H
#define TPUNKT_TASK_H

#include "datastructures/Timestamp.h"

namespace tpunkt
{

using TaskName = FixedString<25>;

struct Task
{
    Task() = default;
    virtual ~Task() = default;

    // Task has no return value we care about - handles errors in its own context
    virtual void invoke();

    // Returns true if the task should be removed
    virtual bool shouldBeRemoved() = 0;

    // Checked after task was invoked
    virtual bool shouldBeInvoked() = 0;

  private:
    TPUNKT_MACROS_STRUCT(Task);
};

template <typename Callable>
struct PeriodicTask final : Task
{
    explicit PeriodicTask(Callable&& func, const uint32_t intervalMicros, const uint32_t limit)
        : Task(), func(std::move(func)), nextExecution(Timestamp::now()), intervalMicros(intervalMicros),
          countLimit(limit)
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
struct SimpleTask final : Task
{
    explicit SimpleTask(Callable&& func) : func(std::move(func))
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
    TPUNKT_MACROS_STRUCT(SimpleTask);
};

} // namespace tpunkt

#endif // TPUNKT_TASK_H