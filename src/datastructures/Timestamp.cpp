// SPDX-License-Identifier: GPL-3.0-only

#include <ctime>
#include "datastructures/Timestamp.h"
#include "util/Logging.h"

namespace tpunkt
{

namespace
{
uint64_t getCurrentMonotonicTime()
{
    timespec time{};
    if(clock_gettime(CLOCK_MONOTONIC_RAW, &time) != 0)
    {
        LOG_CRITICAL("Failed to retrieve time");
        return 0U;
    }
    return static_cast<uint64_t>(time.tv_nsec) + (static_cast<uint64_t>(time.tv_sec) * 1'000'000'000U);
}

} // namespace

Timestamp::Timestamp() : timestamp(getCurrentMonotonicTime())
{
}

Timestamp::Timestamp(const uint64_t& timestamp) : timestamp(timestamp)
{
}

Timestamp Timestamp::operator-(const Timestamp& lhs, const Timestamp& rhs) const
{
    if(lhs.timestamp < rhs.timestamp)
    {
        LOG_FATAL("Invalid operation");
    }
    return Timestamp{lhs.timestamp - rhs.timestamp};
}

bool Timestamp::isInPast() const
{
    return Now().timestamp > timestamp;
}

void Timestamp::addMicros(const uint64_t micros)
{
    timestamp += micros * 1'000U;
}

void Timestamp::addMillis(const uint64_t millis)
{
    timestamp += millis * 1'000'000U;
}

void Timestamp::addSecs(const uint64_t seconds)
{
    timestamp += seconds * 1'000'000'000U;
}

void Timestamp::addMins(const uint64_t minutes)
{
    addSecs(minutes * 60U);
}

void Timestamp::addHours(const uint64_t hours)
{
    addMins(60U * hours);
}

void Timestamp::addDays(const uint64_t days)
{
    addHours(24U * days);
}

void Timestamp::subtractTime(const uint64_t seconds)
{
    timestamp -= seconds * 1'000'000'000;
}

uint64_t Timestamp::getSeconds() const
{
    return timestamp / 1'000'000'000U;
}

void Timestamp::zero()
{
    timestamp = 0U;
}

Timestamp Timestamp::Now()
{
    return {};
}


} // namespace tpunkt