// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_LIMITS_H
#define TPUNKT_LIMITS_H

#include <cstdint>
#include "datastructures/Timestamp.h"

namespace tpunkt
{

struct UserLimit final
{
    UserLimit();

  private:
    uint32_t connectionLimit;
};

struct IPRangeLimit final
{
    constexpr static int32_t TOKENS_PER_SEC = 5;
    constexpr static int32_t TOKENS_MAX = 25;
    constexpr static int32_t TOKEN_COST_CONN = 1;
    constexpr static int32_t TOKEN_MAX_MINUS = -TOKENS_PER_SEC * 60; // 1 min timeout
    constexpr static uint8_t OVER_LIMIT_LIMIT = 15;
    int32_t tokens = TOKENS_MAX;
    uint8_t overLimitRequests = 0;
    Timestamp last;
};

} // namespace tpunkt

#endif // TPUNKT_LIMITS_H