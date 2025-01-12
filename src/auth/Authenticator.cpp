#include <cstdlib>
#include "auth/Authenticator.h"
#include "auth/SessionStorage.h"
#include "auth/AuthToken.h"
#include "util/Memory.h"
#include "datastructures/SecureList.h"

namespace tpunkt
{
    Authenticator::Authenticator()
    {
        sessionStore = TPUNKT_ALLOC(sessionStore, sizeof(int*));
        userStore = TPUNKT_ALLOC(userStore, sizeof(int*));
    }

    Authenticator::~Authenticator()
    {
        TPUNKT_FREE(sessionStore);
        TPUNKT_FREE(userStore);
    }

    AuthToken Authenticator::loginUser(const UserName& name, const Credentials& credentials) {}

    AuthToken Authenticator::authCookie(const SessionID& sessionId) {}

    bool Authenticator::isValid(const AuthToken& token) {}

} // namespace tpunkt