#ifndef TPUNKT_AUTH_TOKEN_H
#define TPUNKT_AUTH_TOKEN_H

#include "fwd.h"

namespace tpunkt
{
    struct AuthToken final
    {
        // Returns the identity this token authenticates
        [[nodiscard]] const User& getUser() const;

    private:
        AuthToken(const User& user, const uint32_t random) : user(user), random(random) {}
        const User& user;
        const uint32_t random; // Random number to make this token non forgeable
        TPUNKT_MACROS_STRUCT(AuthToken);
        friend Authenticator;
    };

} // namespace tpunkt

#endif //TPUNKT_AUTH_TOKEN_H