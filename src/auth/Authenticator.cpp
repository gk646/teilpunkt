#include <cstdlib>
#include <sodium/randombytes.h>
#include "auth/Authenticator.h"
#include "auth/AuthToken.h"

namespace tpunkt
{
    namespace global
    {
        static Authenticator* Authenticator;
    }

    Authenticator::Authenticator()
    {
        TPUNKT_MACROS_GLOBAL_ASSIGN(Authenticator);
    }

    Authenticator::~Authenticator()
    {
        TPUNKT_MACROS_GLOBAL_RESET(Authenticator);
    }

    Authenticator& GetAuthenticator()
    {
        TPUNKT_MACROS_GLOBAL_GET(Authenticator);
    }

    AuthenticatorStatus Authenticator::userLogin(const UserName& name, const Credentials& credentials, AuthToken& token)
    {
        User user{};
        if(userStore.loginUser(name, credentials, user) == false)
        {
            LOG_INFO("Login user failed : Invalid authentication");
            return AuthenticatorStatus::ERR_UNSUCCESSFUL;
        }
        LOG_INFO("Login user success");
        return AuthenticatorStatus::OK;
    }

    AuthenticatorStatus Authenticator::userAdd(const UserName& name, Credentials& credentials)
    {
        if(userStore.contains(name))
        {
            sodium_memzero(&credentials, sizeof(Credentials));
            LOG_INFO("Adding user failed : Name exists");
            return AuthenticatorStatus::ERR_USER_NAME_EXISTS;
        }

        if(userStore.addUser(name, credentials) == false)
        {
            sodium_memzero(&credentials, sizeof(Credentials));
            LOG_INFO("Adding user failed : Unknown");
            return AuthenticatorStatus::ERR_UNSUCCESSFUL;
        }
        sodium_memzero(&credentials, sizeof(Credentials));
        LOG_INFO("Added new user");
        return AuthenticatorStatus::OK;
    }

    AuthenticatorStatus Authenticator::sessionAuth(const SessionID& sessionId, AuthToken& token)
    {
    }

    AuthenticatorStatus Authenticator::isValid(const AuthToken& token)
    {
    }


} // namespace tpunkt