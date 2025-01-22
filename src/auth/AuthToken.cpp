#include "auth/AuthToken.h"
#include "auth/Authenticator.h"

namespace tpunkt
{
    AuthToken::~AuthToken()
    {
        if(GetAuthenticator().tokenInvalidate(*this) != AuthStatus::OK)
        {
            LOG_CRITICAL("Failed to invalidate token");
        }
    }

} // namespace tpunkt