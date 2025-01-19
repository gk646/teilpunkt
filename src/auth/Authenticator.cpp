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

    AuthStatus Authenticator::userLogin(const UserName& name, Credentials& credentials, AuthToken& token)
    {
        SecureEraser eraser{credentials};
        const SecureBox<User>* userBox = nullptr;
        if(userStore.login(name, credentials, userBox) == false || userBox == nullptr)
        {
            LOG_INFO("UserAction : LoginUser : Invalid authentication");
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        sessionStore.add(userBox);

        LOG_INFO("UserAction : LoginUser : Success");
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::userAdd(const UserName& name, Credentials& credentials)
    {
        SecureEraser eraser{credentials};
        if(userStore.nameExists(name))
        {
            LOG_INFO("UserAction : AddUser : Name already exists");
            return AuthStatus::ERR_USER_NAME_EXISTS;
        }
        if(userStore.add(name, credentials) == false)
        {
            LOG_INFO("UserAction : AddUser : Crypto error");
            return AuthStatus::ERR_UNSUCCESSFUL;
        }
        LOG_INFO("UserAction : AddUser : successful");
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::userRemove(const AuthToken& token)
    {
    }

    AuthStatus Authenticator::userChangeCredentials(const AuthToken& token, const UserName& newName,
                                                    Credentials& newCredentials)
    {
    }

    AuthStatus Authenticator::sessionAdd(const AuthToken& token, const SessionData& data, SecureWrapper<SessionID>& out)
    {
    }
    AuthStatus Authenticator::sessionRemove(const AuthToken& token)
    {
    }

    AuthStatus Authenticator::sessionAuth(const SessionID& sessionId, const SessionData& data, AuthToken& out)
    {
    }

    bool Authenticator::tokenValid(const AuthToken& token)
    {
    }

    AuthStatus Authenticator::tokenInvalidate(AuthToken& token)
    {
    }

} // namespace tpunkt