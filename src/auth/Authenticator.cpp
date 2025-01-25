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
        SpinlockGuard lock{authLock};
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
        SpinlockGuard lock{authLock};
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
        SpinlockGuard lock{authLock};
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
        SpinlockGuard lock{authLock};
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
        SpinlockGuard lock{authLock};
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, SessionAdd, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }

        SessionID sessionId;
        if(!sessionStore.add(token.userID, data, sessionId))
        {
            LOG_EVENT(UserAction, SessionAdd, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        // Assign session id
        auto reader = out.get();
        reader.get() = sessionId;

        LOG_EVENT(UserAction, SessionAdd, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::sessionRemove(const AuthToken& token)
    {
        SpinlockGuard lock{authLock};
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, SessionRemove, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }
    }

    AuthStatus Authenticator::sessionAuth(const SessionID& sessionId, const SessionMetaData& data, AuthToken& out)
    {
        SpinlockGuard lock{authLock};
        uint32_t userID = 0U;
        if(!sessionStore.get(sessionId, data, userID))
        {
            LOG_EVENT(UserAction, SessionAuthenticate, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        uint32_t random = 0U;
        if(!sessionStore.addToken(userID, random))
        {
            LOG_EVENT(UserAction, SessionAuthenticate, Generic_Unsuccessful);
            return AuthStatus::ERR_UNSUCCESSFUL;
        }

        out.random = random;
        out.userID = userID;

        LOG_EVENT(UserAction, SessionAuthenticate, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::sessionGet(const AuthToken& token)
    {
        SpinlockGuard lock{authLock};
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, SessionGetSessions, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }
    }

    bool Authenticator::tokenValid(const AuthToken& token)
    {
        if(!authLock.isLocked()) // Can be called within lock or standalone
        {
            SpinlockGuard lock{authLock};
            return sessionStore.tokenValid(token);
        }
        return sessionStore.tokenValid(token);
    }

    AuthStatus Authenticator::tokenInvalidate(AuthToken& consumed)
    {
        SpinlockGuard lock{authLock};
        SecureEraser eraser{consumed};
        if(!sessionStore.removeToken(consumed))
        {
            LOG_EVENT(InternalCode, TokenInvalidate, Invalid_Token);
            LOG_CRITICAL("Failed to invalidate token");
            return AuthStatus::ERR_UNSUCCESSFUL;
        }
        LOG_EVENT(InternalCode, TokenInvalidate, Successful);
        return AuthStatus::OK;
    }

    AuthStatus Authenticator::getUserName(const AuthToken& token, UserName& out)
    {
        SpinlockGuard lock{authLock};
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserDataGetName, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }
    }

    AuthStatus Authenticator::getWrappedKey(const AuthToken& token, FileHandle handle, SecureWrapper<WrappedKey>& out)
    {
        SpinlockGuard lock{authLock};
        if(!tokenValid(token))
        {
            LOG_EVENT(UserAction, UserDataGetWrappedKey, Invalid_Token);
            return AuthStatus::ERR_INVALID_TOKEN;
        }
    }

} // namespace tpunkt