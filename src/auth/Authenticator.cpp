#include <cstdlib>
#include <sodium/randombytes.h>
#include "auth/Authenticator.h"
#include "auth/AuthToken.h"
#include "datastructures/SecureEraser.h"

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

    AuthenticatorStatus Authenticator::userLogin(const UserName& name, Credentials& credentials, AuthToken& token)
    {
        SecureEraser eraser{credentials};
        const SecureBox<User>* userBox = nullptr;
        if(userStore.login(name, credentials, userBox) == false || userBox == nullptr)
        {
            LOG_INFO("UserAction : LoginUser : Invalid authentication");
            return AuthenticatorStatus::ERR_UNSUCCESSFUL;
        }


        sessionStore.add(userBox);

        LOG_INFO("UserAction : LoginUser : Success");
        return AuthenticatorStatus::OK;
    }

    AuthenticatorStatus Authenticator::userLogout(const AuthToken& token)
    {

    }

    AuthenticatorStatus Authenticator::userAdd(const UserName& name, Credentials& credentials)
    {
        SecureEraser eraser{credentials};
        if(userStore.nameExists(name))
        {
            LOG_INFO("UserAction : AddUser : Name already exists");
            return AuthenticatorStatus::ERR_USER_NAME_EXISTS;
        }
        if(userStore.add(name, credentials) == false)
        {
            LOG_INFO("UserAction : AddUser : Crypto error");
            return AuthenticatorStatus::ERR_UNSUCCESSFUL;
        }
        LOG_INFO("UserAction : AddUser : successful");
        return AuthenticatorStatus::OK;
    }

    AuthenticatorStatus Authenticator::removeUser(const AuthToken& token)
    {
    }

    AuthenticatorStatus Authenticator::userChangeCredentials(const AuthToken& token, Credentials& newCredentials)
    {
    }

    AuthenticatorStatus Authenticator::sessionRemove(const AuthToken& token)
    {
    }

    AuthenticatorStatus Authenticator::sessionAuth(const SessionID& sessionId, AuthToken& token)
    {
    }

    bool Authenticator::tokenValid(const AuthToken& token)
    {
    }
    AuthenticatorStatus Authenticator::tokenInvalidate(AuthToken& token)
    {
    }

} // namespace tpunkt