#ifndef TPUNKT_AUTH_TOKEN_H
#define TPUNKT_AUTH_TOKEN_H

#include "fwd.h"
#include "datastructures/SecureWrapper.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{
    struct AuthToken final
    {
        // Returns the identity this token authenticates
        [[nodiscard]] const SecureBox<User>& getUserBox() const;

        AuthToken() = default;
        ~AuthToken();

      private:
        const SecureBox<User>* userBox = nullptr; // User this token authenticates
        const uint32_t random = 0;                // Random number to make this token non forgeable
        TPUNKT_MACROS_STRUCT(AuthToken);
        friend Authenticator;
    };

} // namespace tpunkt

#endif // TPUNKT_AUTH_TOKEN_H