// SPDX-License-Identifier: GPL-3.0-only

#include "auth/Authenticator.h"
#include "datastructures/SecureEraser.h"
#include "instance/InstanceConfig.h"
#include "util/Strings.h"
#include "server/DTO.h"

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

AuthStatus Authenticator::userAdd(const UserID actor, const UserName& name, Credentials& out)
{
    constexpr auto action = EventAction::UserAdd;
    SpinlockGuard lock{authLock};

    if(!IsValidUserName(name) || !IsValidPassword(out.password))
    {
        LOG_EVENT_AUTH(actor, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(GetInstanceConfig().getBool(BoolParamKey::USER_ONLY_ADMIN_CREATE_ACCOUNT))
    {
        LOG_EVENT_AUTH(actor, FAIL_CONFIG_RESTRICTED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    if(userStore.nameExists(name))
    {
        LOG_EVENT_AUTH(actor, FAIL_USERNAME_EXISTS, AuthenticationEventData{}); // We report this
        return AuthStatus::ERR_USER_NAME_EXISTS;
    }

    if(!userStore.add(name, out))
    {
        LOG_EVENT_AUTH(actor, WARN_FAILED_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userLoginPassword( DTO::RequestUserLoginPassword& consumed, UserID& user)
{
    constexpr auto action = EventAction::UserLogin;
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!userStore.loginPassword(consumed, user))
    {
        LOG_EVENT_AUTH(UserID::SERVER, FAIL_INVALID_CREDENTIALS, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(user, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userRemove(const UserID actor, const UserID user)
{
    constexpr auto action = EventAction::UserRemove;
    SpinlockGuard lock{authLock};
    if(actor != user && getIsAdmin(actor) != AuthStatus::OK)
    {
        LOG_EVENT_AUTH(actor, FAIL_NO_ADMIN, AuthenticationEventData{});
        return AuthStatus::ERR_NO_ADMIN;
    }

    if(!userStore.remove(user))
    {
        LOG_EVENT_AUTH(actor, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::userChangeCredentials(const UserID actor, const UserName& newName, Credentials& consumed)
{
    constexpr auto action = EventAction::UserChangeCredentials;
    SpinlockGuard lock{authLock};
    SecureEraser eraser{consumed};

    if(!IsValidUserName(newName) || !IsValidPassword(consumed.password))
    {
        LOG_EVENT_AUTH(actor, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    if(!userStore.changeCredentials(actor, newName, consumed))
    {
        LOG_EVENT_AUTH(actor, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}


AuthStatus Authenticator::sessionAdd(const UserID actor, const SessionMetaData& metaData,
                                     SecureWrapper<SessionToken>& out)
{
    constexpr auto action = EventAction::SessionAdd;
    SpinlockGuard lock{authLock};

    if(actor == UserID::INVALID) [[unlikely]]
    {
        LOG_EVENT_AUTH(actor, FAIL_INVALID_ARGUMENTS, AuthenticationEventData{});
        return AuthStatus::ERR_INVALID_ARGUMENTS;
    }

    SessionToken sessionId;
    if(!sessionStore.add(actor, metaData, sessionId))
    {
        LOG_EVENT_AUTH(actor, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    // Assign session id
    auto reader = out.get();
    reader.get() = sessionId;

    LOG_EVENT_AUTH(actor, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionRemove(const UserID actor, const Timestamp& creation)
{
    constexpr auto action = EventAction::SessionRemove;
    SpinlockGuard lock{authLock};

    if(!sessionStore.remove(actor, creation))
    {
        LOG_EVENT_AUTH(actor, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionAuth(const UserID lookup, const SessionToken& sessionId,
                                      const SessionMetaData& metaData, UserID& user)
{
    constexpr auto action = EventAction::SessionAuthenticate;
    SpinlockGuard lock{authLock};
    if(!sessionStore.get(lookup, sessionId, metaData, user))
    {
        LOG_EVENT_AUTH(lookup, FAIL_UNSPECIFIED, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(lookup, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::sessionGetInfo(const UserID actor, std::vector<DTO::SessionInfo>& collector)
{
    constexpr auto action = EventAction::SessionGetInfo;
    SpinlockGuard lock{authLock};

    if(!sessionStore.getInfo(actor, collector))
    {
        LOG_EVENT_AUTH(actor, FAIL_NO_SUCH_USER, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(actor, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::getUserName(const UserID user, UserName& out)
{
    constexpr auto action = EventAction::UserDataGetName;
    SpinlockGuard lock{authLock};

    if(!userStore.getName(user, out))
    {
        LOG_EVENT_AUTH(user, FAIL_NO_SUCH_USER, AuthenticationEventData{});
        return AuthStatus::ERR_UNSUCCESSFUL;
    }

    LOG_EVENT_AUTH(user, INFO_SUCCESS, AuthenticationEventData{});
    return AuthStatus::OK;
}

AuthStatus Authenticator::getIsAdmin(UserID user)
{
    // TODO
    SpinlockGuard lock{authLock};
    if(user == UserID::SERVER)
    {
        return AuthStatus::OK;
    }
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
