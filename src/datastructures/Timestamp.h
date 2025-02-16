// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_TIMESTAMP_H
#define TPUNKT_TIMESTAMP_H

#include <cstdint>

namespace tpunkt
{
struct Timestamp
{
    Timestamp();

    // Returns true if this timestamp represents a time before the current moment
    [[nodiscard]] bool isInPast() const;

    void addMicros(uint64_t micros);
    void addMillis(uint64_t millis);
    void addSecs(uint64_t seconds);
    void addMins(uint64_t minutes);
    void addHours(uint64_t hours);
    void addDays(uint64_t days);

    void subtractTime(uint64_t seconds);

    // zeros the timestamp - always expires
    void zero();

    static Timestamp now();

  private:
    uint64_t timestamp;
};

} // namespace tpunkt

#endif // TPUNKT_TIMESTAMP_H