// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_AUTH_TOKEN_H
#define TPUNKT_AUTH_TOKEN_H

#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"

namespace tpunkt
{
struct AuthToken final
{
    AuthToken() = default;
    ~AuthToken();

    [[nodiscard]] UserID getUser() const;

  private:
    UserID userID{};   // Which user this token authenticates
    uint32_t random{}; // Random number to make this token non forgeable
    TPUNKT_MACROS_STRUCT(AuthToken);
    friend Authenticator;
    friend SessionStorage;
};

} // namespace tpunkt

#endif // TPUNKT_AUTH_TOKEN_H