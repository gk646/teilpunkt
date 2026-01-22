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

Authenticator& Authenticator::GetInstance()
{
    TPUNKT_MACROS_GLOBAL_GET(Authenticator);
}

AuthStatus Authenticator::userAdd(const UserID actor, const UserName& name, Credentials& consumed)
{
    constexpr auto event = EventAction::UserAdd;
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!IsValidUserName(name) || !IsValidPassword(consumed.password))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(GetInstanceConfig().getBool(BoolParamKey::USER_ONLY_ADMIN_CREATE_ACCOUNT))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_CONFIG_RESTRICTED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    if(userStore.nameExists(name))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_USERNAME_EXISTS, AuthenticationEventData{}); // We report this
        return AuthStatus::ERR_USER_NAME_EXISTS;
    }

    if(!userStore.add(name, consumed))
    {
        LOG_EVENT_AUTH(actor, event, WARN_FAILED_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userLogin(const UserName& name, Credentials& consumed, UserID& user)
{
    constexpr auto event = EventAction::UserLogin;
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!userStore.login(name, consumed, user))
    {
        LOG_EVENT_AUTH(UserID::SERVER, event, FAIL_INVALID_CREDENTIALS, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(user, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userRemove(const UserID actor, const UserID user)
{
    constexpr auto event = EventAction::UserRemove;
    SpinlockGuard lock{authLock};
    if(actor != user && getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT_AUTH(actor, event, FAIL_NO_ADMIN, AuthenticationEventData{});
        return AuthStatus::ERR_NO_ADMIN;
    }

    if(!userStore.remove(user))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userChangeCredentials(const UserID actor, const UserName& newName, Credentials& consumed)
{
    constexpr auto event = EventAction::UserChangeCredentials;
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!IsValidUserName(newName) || !IsValidPassword(consumed.password))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(!userStore.changeCredentials(actor, newName, consumed))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}


AuthStatus Authenticator::sessionAdd(const UserID actor, const SessionMetaData& metaData,
                                     SecureWrapper<SessionToken>& out)
{
    constexpr auto event = EventAction::SessionAdd;
    SpinlockGuard lock{authLock};

    if(actor == UserID::INVALID) [[unlikely]]
    {
        LOG_EVENT_AUTH(actor, event, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    SessionToken sessionId;
    if(!sessionStore.add(actor, metaData, sessionId))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    // Assign session id
    auto reader = out.get();
    reader.get() = sessionId;

    LOG_EVENT_AUTH(actor, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionRemove(const UserID actor, const Timestamp& creation)
{
    constexpr auto event = EventAction::SessionRemove;
    SpinlockGuard lock{authLock};

    if(!sessionStore.remove(actor, creation))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionAuth(const UserID lookup, const SessionToken& sessionId,
                                      const SessionMetaData& metaData, UserID& user)
{
    constexpr auto event = EventAction::SessionAuthenticate;
    SpinlockGuard lock{authLock};
    if(!sessionStore.get(lookup, sessionId, metaData, user))
    {
        LOG_EVENT_AUTH(lookup, event, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(user, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionGetInfo(const UserID actor, std::vector<DTO::SessionInfo>& collector)
{
    constexpr auto event = EventAction::SessionGetInfo;
    SpinlockGuard lock{authLock};

    if(!sessionStore.getInfo(actor, collector))
    {
        LOG_EVENT_AUTH(actor, event, FAIL_NO_SUCH_USER, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::getUserName(const UserID user, UserName& out)
{
    constexpr auto event = EventAction::UserDataGetName;
    SpinlockGuard lock{authLock};

    if(!userStore.getName(user, out))
    {
        LOG_EVENT_AUTH(user, event, FAIL_NO_SUCH_USER, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(user, event, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::getIsAdmin(UserID user)
{
    SpinlockGuard lock{authLock};
    // TODO
    if(user == UserID::SERVER)
    {
        return AuthStatus::OK;
    }
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
