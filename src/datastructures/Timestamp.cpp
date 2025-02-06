#include <ctime>
#include "util/Logging.h"
#include "datastructures/Timestamp.h"

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
            return static_cast<uint64_t>(time.tv_nsec);
        }
    } // namespace

    Timestamp::Timestamp() : timestamp(getCurrentMonotonicTime())
    {
    }

    bool Timestamp::inThePast() const
    {
        return now().timestamp > timestamp;
    }

    void Timestamp::addSecs(const uint64_t seconds)
    {
        timestamp += seconds * 1'000'000'000'000U;
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
        timestamp -= seconds * 1'000'000'000'000U;
    }

    void Timestamp::zero()
    {
        timestamp = 0U;
    }

    Timestamp Timestamp::now()
    {
        return {};
    }


} // namespace tpunkt