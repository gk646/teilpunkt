#include <cstdlib>
#include <sodium.h>
#include "auth/Authenticator.h"
#include "auth/SessionStorage.h"
#include "auth/AuthToken.h"
#include "util/Memory.h"
#include "datastructures/SecureList.h"
#include "common/User.h"

namespace tpunkt
{
    static Authenticator* AUTH;

    Authenticator::Authenticator()
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN(AUTH);
        sessionStore = TPUNKT_ALLOC(sessionStore, sizeof(int*));
        userStore = TPUNKT_ALLOC(userStore, sizeof(int*));
        LOG_DEBUG("Authenticator initialized");
    }

    Authenticator::~Authenticator()
    {
        TPUNKT_FREE(sessionStore);
        TPUNKT_FREE(userStore);
        TPUNKT_MACROS_GLOBAL_RESET(AUTH);
        LOG_DEBUG("Authenticator exited");
    }

    Authenticator& GetAuthenticator()
    {
        TPUNKT_MACROS_GLOBAL_GET(AUTH);
    }



    AuthToken Authenticator::loginUser(const UserName& name, const Credentials& credentials)
    {
        User user{};
        return AuthToken{ user, randombytes_random()};
    }

    AuthToken Authenticator::authCookie(const SessionID& sessionId)
    {

    }

    bool Authenticator::isValid(const AuthToken& token) {}


} // namespace tpunkt