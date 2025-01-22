#include "auth/Authenticator.h"
#include "auth/AuthToken.h"
#include "datastructures/SecureEraser.h"
#include "instance/InstanceConfig.h"

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

    AuthStatus Authenticator::userLogin(const UserName& name, Credentials& consumed, AuthToken& out)
    {
        SecureEraser eraser{consumed};
        const SecureBox<User>* userBox = nullptr;
        if(!userStore.login(name, consumed, userBox) || userBox == nullptr)
        {
            LOG_EVENT(UserAction, UserLogin, Invalid_Authentication);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        uint32_t random{};
        if(!sessionStore.addToken(*userBox, random))
        {
            LOG_EVENT(UserAction, UserLogin, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }
        out.random = random;
        out.userBox = userBox;

        LOG_EVENT(UserAction, UserLogin, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::userAdd(const UserName& name, Credentials& consumed)
    {
        SecureEraser eraser{consumed};
        if(GetInstanceConfig().getBool(BoolParamKey::USER_ONLY_ADMIN_CREATE_ACCOUNT))
        {
            LOG_EVENT(UserAction, UserAdd, Feature_Disabled);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        if(userStore.nameExists(name))
        {
            LOG_EVENT(UserAction, UserAdd, Username_Already_Exists); // We report this
            return AuthStatus::ERR_USER_NAME_EXISTS;
        }

        if(!userStore.add(name, consumed))
        {
            LOG_EVENT(UserAction, UserAdd, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        LOG_EVENT(UserAction, UserAdd, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::userRemove(const AuthToken& token)
    {
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserRemove, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }

        const auto userBox = token.getUserBox()->get(); // Never null after token is valid
        if(!userStore.remove(userBox.get().name))
        {
            LOG_EVENT(UserAction, UserRemove, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        LOG_EVENT(UserAction, UserRemove, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::userChangeCredentials(const AuthToken& token, const UserName& newName,
                                                    Credentials& consumed)
    {
        SecureEraser eraser{consumed};
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserChangeCredentials, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }

        return AuthStatus::OK;
        // userStore.changeCredentials();
    }

    AuthStatus Authenticator::sessionAdd(const AuthToken& token, const SessionMetaData& data,
                                         SecureWrapper<SessionID>& out)
    {
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserSessionAdd, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }
    }

    AuthStatus Authenticator::sessionRemove(const AuthToken& token)
    {
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserSessionAdd, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }
    }

    AuthStatus Authenticator::sessionAuth(const SessionID& sessionId, const SessionMetaData& data, AuthToken& out)
    {
    }

    bool Authenticator::tokenValid(const AuthToken& token) const
    {
        return sessionStore.tokenValid(token);
    }

    AuthStatus Authenticator::tokenInvalidate(AuthToken& token)
    {
        SecureEraser eraser{token};
        if(!tokenValid(token))
        {
            LOG_EVENT(InternalCode, TokenInvalidate, Successful);
            return AuthStatus::OK;                                       // Already invalid
        }

        if(!sessionStore.removeToken(*token.getUserBox(), token.random)) // Valid after token is valid
        {
            LOG_EVENT(InternalCode, TokenInvalidate, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }
        LOG_EVENT(InternalCode, TokenInvalidate, Successful);
        return AuthStatus::OK;
    }

} // namespace tpunkt