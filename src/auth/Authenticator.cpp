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

Authenticator& Authenticator::GetInstance()
{
    TPUNKT_MACROS_GLOBAL_GET(Authenticator);
}

const char* Authenticator::GetStatusStr(const AuthStatus status)
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
        case AuthStatus::ERR_NO_ADMIN:
            return "No admin";
    }
    return nullptr;
}

AuthStatus Authenticator::userAdd(const UserID actor, const UserName& name, Credentials& consumed)
{
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!IsValidUserName(name) || !IsValidPassword(consumed.password))
    {
        LOG_EVENT(actor, Users, UserAdd, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(GetInstanceConfig().getBool(BoolParamKey::USER_ONLY_ADMIN_CREATE_ACCOUNT))
    {
        LOG_EVENT(actor, Users, UserAdd, FAIL_CONFIG_RESTRICTED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    if(userStore.nameExists(name))
    {
        LOG_EVENT(actor, Users, UserAdd, FAIL_USERNAME_EXISTS, AuthenticationEventData{}); // We report this
        return AuthStatus::ERR_USER_NAME_EXISTS;
    }

    if(!userStore.add(name, consumed))
    {
        LOG_EVENT(actor, Users, UserAdd, WARN_FAILED_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(actor, Users, UserAdd, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userLogin(const UserName& name, Credentials& consumed, UserID& user)
{
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!userStore.login(name, consumed, user))
    {
        LOG_EVENT(UserID::SERVER, Users, UserLogin, FAIL_INVALID_CREDENTIALS, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(user, Users, UserLogin, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userRemove(const UserID actor, const UserID user)
{
    SpinlockGuard lock{authLock};
    if(actor != user && getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT(actor, Users, UserRemove, FAIL_NO_ADMIN, AuthenticationEventData{});
        return AuthStatus::ERR_NO_ADMIN;
    }

    if(!userStore.remove(user))
    {
        LOG_EVENT(actor, Users, UserRemove, INFO_FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(actor, Users, UserRemove, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userChangeCredentials(const UserID actor, const UserName& newName, Credentials& consumed)
{
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!IsValidUserName(newName) || !IsValidPassword(consumed.password))
    {
        LOG_EVENT(actor, Users, UserChangeCredentials, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(!userStore.changeCredentials(actor, newName, consumed))
    {
        LOG_EVENT(actor, Users, UserChangeCredentials, INFO_FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(actor, Users, UserChangeCredentials, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}


AuthStatus Authenticator::sessionAdd(const UserID actor, const SessionMetaData& metaData,
                                     SecureWrapper<SessionToken>& out)
{
    SpinlockGuard lock{authLock};

    if(actor == UserID::INVALID) [[unlikely]]
    {
        LOG_EVENT(actor, Users, SessionAdd, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    SessionToken sessionId;
    if(!sessionStore.add(actor, metaData, sessionId))
    {
        LOG_EVENT(actor, Users, SessionAdd, INFO_FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    // Assign session id
    auto reader = out.get();
    reader.get() = sessionId;

    LOG_EVENT(actor, Users, SessionAdd, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionRemove(const UserID actor, const Timestamp& creation)
{
    SpinlockGuard lock{authLock};

    if(!sessionStore.remove(actor, creation))
    {
        LOG_EVENT(actor, Users, SessionRemove, INFO_FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(actor, Users, SessionRemove, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionAuth(const UserID lookup, const SessionToken& sessionId,
                                      const SessionMetaData& metaData, UserID& user)
{
    SpinlockGuard lock{authLock};
    if(!sessionStore.get(lookup, sessionId, metaData, user))
    {
        LOG_EVENT(UserID::SERVER, Users, SessionAuthenticate, INFO_FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT(user, Users, SessionAuthenticate, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionGetInfo(const UserID actor, std::vector<DTO::SessionInfo>& collector)
{
    SpinlockGuard lock{authLock};
    sessionStore.getInfo(actor, collector);
    LOG_EVENT(actor, Users, SessionGetInfo, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::getUserName(const UserID user, UserName& out)
{
    SpinlockGuard lock{authLock};

    // TODO
    LOG_FATAL("Not implemented");
    return AuthStatus::INVALID;
}

AuthStatus Authenticator::getIsAdmin(UserID user)
{
    SpinlockGuard lock{authLock};
    // TODO
    LOG_FATAL("Not implemented");
    return AuthStatus::INVALID;
}

Authenticator::Authenticator()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(Authenticator);
}

Authenticator::~Authenticator()
{
    TPUNKT_MACROS_GLOBAL_RESET(Authenticator);
}


} // namespace tpunkt
