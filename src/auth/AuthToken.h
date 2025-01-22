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

      private:
        uint32_t random{}; // Random number to make this token non forgeable
        uint32_t userID{}; // Which user this token authenticates
        TPUNKT_MACROS_STRUCT(AuthToken);
        friend Authenticator;
        friend SessionStorage;
    };

} // namespace tpunkt

#endif // TPUNKT_AUTH_TOKEN_H