#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"
#include "auth/AuthToken.h"
#include "auth/Authenticator.h"

namespace tpunkt
{
    const SecureBox<User>& AuthToken::getUserBox() const
    {
        if(userBox != nullptr) [[likely]]
        {
            return *userBox;
        }
        LOG_FATAL("No box present");
    }

    AuthToken::~AuthToken()
    {
        GetAuthenticator().tokenInvalidate(*this);
    }

} // namespace tpunkt