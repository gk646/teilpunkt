#ifndef TPUNKT_AUTH_TOKEN_H
#define TPUNKT_AUTH_TOKEN_H

#include "fwd.h"

namespace tpunkt
{
    struct AuthToken final
    {
        // Returns the identity this token authenticates
        [[nodiscard]] const SecureBox<User>& getUserBox() const;

        ~AuthToken();

      private:
        AuthToken(const SecureBox<User>& usrBox, uint32_t rand);

        const SecureBox<User>& userBox; // User this token authenticates
        const uint32_t random;          // Random number to make this token non forgeable
        TPUNKT_MACROS_STRUCT(AuthToken);
        friend Authenticator;
    };

} // namespace tpunkt

#endif // TPUNKT_AUTH_TOKEN_H