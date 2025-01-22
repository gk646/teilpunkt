#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"
#include "auth/AuthToken.h"
#include "auth/Authenticator.h"

namespace tpunkt
{
    const SecureBox<User>* AuthToken::getUserBox() const
    {
        return userBox;
    }

    AuthToken::~AuthToken()
    {
        if(GetAuthenticator().tokenInvalidate(*this) != AuthStatus::OK)
        {
            LOG_CRITICAL("Failed to invalidate token");
        }
    }

} // namespace tpunkt