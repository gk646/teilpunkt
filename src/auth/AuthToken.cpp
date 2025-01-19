#include <cstdint>
#include "fwd.h"
#include "util/Macros.h"
#include "auth/AuthToken.h"
#include "auth/Authenticator.h"

namespace tpunkt
{
    const SecureBox<User>& AuthToken::getUserBox() const
    {
        return userBox;
    }

    AuthToken::AuthToken(const SecureBox<User>& usrBox, const uint32_t rand)
        : userBox(usrBox), random(rand)
    {
    }

    AuthToken::~AuthToken()
    {
        GetAuthenticator().tokenInvalidate(*this);
    }

} // namespace tpunkt