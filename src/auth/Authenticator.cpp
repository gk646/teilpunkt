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

    AuthStatus Authenticator::userLogin(const UserName& name, Credentials& consumed, AuthToken& out)
    {
        SecureEraser eraser{consumed};

        uint32_t userID{};
        if(!userStore.login(name, consumed, userID))
        {
            LOG_EVENT(UserAction, UserLogin, Invalid_Authentication);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        uint32_t random{};
        if(!sessionStore.addToken(userID, random))
        {
            LOG_EVENT(UserAction, UserLogin, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        out.random = random;
        out.userID = userID;
        LOG_EVENT(UserAction, UserLogin, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::userRemove(const AuthToken& token)
    {
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserRemove, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }

        if(!userStore.remove(token.userID))
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

        if(!userStore.changeCredentials(token.userID, newName, consumed))
        {
            LOG_EVENT(UserAction, UserChangeCredentials, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        LOG_EVENT(UserAction, UserChangeCredentials, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::sessionAdd(const AuthToken& token, const SessionMetaData& data,
                                         SecureWrapper<SessionID>& out)
    {
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserSessionAdd, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }

        SessionID sessionId;
        if(!sessionStore.add(token.userID, data, sessionId))
        {
            LOG_EVENT(UserAction, UserSessionAdd, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        // Assign session id
        auto reader = out.get();
        reader.get() = sessionId;

        LOG_EVENT(UserAction, UserSessionAdd, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::sessionRemove(const AuthToken& token)
    {
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserSessionRemove, Invalid_Token);
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

    AuthStatus Authenticator::tokenInvalidate(AuthToken& consumed)
    {
        SecureEraser eraser{consumed};
        if(!tokenValid(consumed))
        {
            LOG_EVENT(InternalCode, TokenInvalidate, Successful);
            return AuthStatus::OK;
        }

        if(!sessionStore.removeToken(consumed))
        {
            LOG_EVENT(InternalCode, TokenInvalidate, Generic_Unsuccessful);
            LOG_CRITICAL("Failed to invalidate token");
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        LOG_EVENT(InternalCode, TokenInvalidate, Successful);
        return AuthStatus::OK;
    }

} // namespace tpunkt