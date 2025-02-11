// SPDX-License-Identifier: Apache License 2.0

#include "auth/Authenticator.h"
#include "auth/AuthToken.h"

namespace tpunkt
{
AuthToken::~AuthToken()
{
    if(GetAuthenticator().tokenInvalidate(*this) != AuthStatus::OK)
    {
        LOG_CRITICAL("Failed to invalidate token");
    }
}

UserID AuthToken::getUser() const
{
    return userID;
}

} // namespace tpunkt