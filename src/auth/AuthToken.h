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
        AuthToken(const User& usr, const uint32_t rand) : user(usr), random(rand) {}
        const User& user;
        const uint32_t random; // Random number to make this token non forgeable
        TPUNKT_MACROS_STRUCT(AuthToken);
        friend Authenticator;
    };

} // namespace tpunkt

#endif //TPUNKT_AUTH_TOKEN_H