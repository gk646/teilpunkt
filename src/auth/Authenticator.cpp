// SPDX-License-Identifier: GPL-3.0-only

#include "auth/Authenticator.h"
#include "datastructures/SecureEraser.h"
#include "instance/InstanceConfig.h"
#include "util/Strings.h"

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

const char* GetAuthStatusStr(const AuthStatus status)
{
    switch(status)
    {
        case AuthStatus::INVALID:
            return "Invalid";
        case AuthStatus::OK:
            return "OK";
        case AuthStatus::ERR_UNSUCCESSFUL:
            return "Unsuccessful operation";
        case AuthStatus::ERR_USER_NAME_EXISTS:
            return "User name exists";
        case AuthStatus::ERR_INVALID_ARGUMENTS:
            return "Invalid arguments";
    }
    return nullptr;
}

AuthStatus Authenticator::userAdd(const UserName& name, Credentials& consumed)
{
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!IsValidUserName(name) || !IsValidPassword(consumed.password))
    {
        LOG_EVENT(UserAction, UserAdd, FAIL_INVALID_ARGUMENTS);
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(GetInstanceConfig().getBool(BoolParamKey::USER_ONLY_ADMIN_CREATE_ACCOUNT))
    {
        LOG_EVENT(UserAction, UserAdd, FAIL_CONFIG_RESTRICTED);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    if(userStore.nameExists(name))
    {
        LOG_EVENT(UserAction, UserAdd, FAIL_USERNAME_EXISTS); // We report this
        return AuthStatus::ERR_USER_NAME_EXISTS;
    }

    if(!userStore.add(name, consumed))
    {
        LOG_EVENT(UserAction, UserAdd, WARN_OPERATION_FAILED);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, UserAdd, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::userLogin(const UserName& name, Credentials& consumed, UserID& user)
{
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!userStore.login(name, consumed, user))
    {
        LOG_EVENT(UserAction, UserLogin, FAIL_INVALID_CREDENTIALS);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, UserLogin, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::userRemove(const UserID actor, const UserID user)
{
    SpinlockGuard lock{authLock};
    if(actor != user && GetAuthenticator().getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT(UserAction, UserRemove, FAIL_NO_ADMIN);
        return AuthStatus::ERR_NO_ADMIN;
    }

    if(!userStore.remove(user))
    {
        LOG_EVENT(UserAction, UserRemove, FAIL_SERVER_OPERATION);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, UserRemove, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::userChangeCredentials(const UserID user, const UserName& newName, Credentials& consumed)
{
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!userStore.changeCredentials(user, newName, consumed))
    {
        LOG_EVENT(UserAction, UserChangeCredentials, FAIL_SERVER_OPERATION);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, UserChangeCredentials, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionAdd(const UserID user, const SessionMetaData& data, SecureWrapper<SessionToken>& out)
{
    SpinlockGuard lock{authLock};

    SessionToken sessionId;
    if(!sessionStore.add(user, data, sessionId))
    {
        LOG_EVENT(UserAction, SessionAdd, FAIL_SERVER_OPERATION);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    // Assign session id
    auto reader = out.get();
    reader.get() = sessionId;

    LOG_EVENT(UserAction, SessionAdd, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionRemove(const UserID user, const Timestamp& creation)
{
    SpinlockGuard lock{authLock};

    if(!sessionStore.remove(user, creation))
    {
        LOG_EVENT(UserAction, SessionRemove, FAIL_SERVER_OPERATION);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, SessionRemove, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionAuth(const SessionToken& sessionId, const SessionMetaData& data, UserID& user)
{
    SpinlockGuard lock{authLock};
    if(!sessionStore.get(sessionId, data, user))
    {
        LOG_EVENT(UserAction, SessionAuthenticate, FAIL_SERVER_OPERATION);
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(UserAction, SessionAuthenticate, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionGetInfo(const UserID user, std::vector<DTOSessionInfo>& collector)
{
    SpinlockGuard lock{authLock};
    sessionStore.getInfo(user, collector);
    LOG_EVENT(UserAction, SessionGetInfo, SUCCESS);
    return AuthStatus::OK;
}

AuthStatus Authenticator::getUserName(const UserID user, UserName& out)
{
    SpinlockGuard lock{authLock};
    // TODo
    return AuthStatus::INVALID;
}


AuthStatus Authenticator::getIsAdmin(UserID user)
{
    SpinlockGuard lock{authLock};
    // TODO
    return AuthStatus::INVALID;
}

} // namespace tpunkt