// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_TIMESTAMP_H
#define TPUNKT_TIMESTAMP_H

#include <cstdint>

namespace tpunkt
{

struct Timestamp
{
    Timestamp();

    Timestamp operator-(const Timestamp& rhs) const;
    bool operator==(const Timestamp& rhs) const;

    // Returns true if this timestamp represents a time before the current moment
    [[nodiscard]] bool isInPast() const;

    void addMicros(uint64_t micros);
    void addMillis(uint64_t millis);
    void addSecs(uint64_t seconds);
    void addMins(uint64_t minutes);
    void addHours(uint64_t hours);
    void addDays(uint64_t days);

    void subtractTime(uint64_t seconds);

    [[nodiscard]] uint64_t getSeconds() const;

    // zeros the timestamp - always expires
    void zero();

    static Timestamp Now(uint64_t addSeconds = 0);

  private:
    explicit Timestamp(const uint64_t& timestamp);

    uint64_t timestamp;
};

} // namespace tpunkt

#endif // TPUNKT_TIMESTAMP_H