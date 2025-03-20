// SPDX-License-Identifier: GPL-3.0-only

#include <sodium/randombytes.h>
#include "auth/SessionStorage.h"
#include "instance/InstanceConfig.h"

namespace tpunkt
{

Session::Session(const SessionMetaData& metaData)
    : metaData(metaData),
      expiration(Timestamp::Now(GetInstanceConfig().getNumber(NumberParamKey::USER_SESSION_EXPIRATION_DELAY_SECS)))
{
    randombytes_buf(token.data(), token.capacity());
}

Session::Session(Session&& other) noexcept
    : token(other.token), metaData(other.metaData), creation(other.creation), expiration(other.expiration)
{
}

bool Session::isValid(const SessionMetaData& metaData) const
{
    if(this->metaData != metaData)
    {
        return false;
    }

    if(expiration.isInPast())
    {
        return false;
    }

    return true;
}

UserSessionData::UserSessionData(const UserID user) : user(user)
{
}

bool SessionStorage::add(const UserID user, const SessionMetaData& metaData, SessionToken& token)
{
    UserSessionData* userData = getUserSessionData(user);
    if(userData == nullptr)                                                              // User is not present yet
    {
        userData = &sessions.emplace_back(user);
    }

    auto sessionList = userData->sessions.get();
    const auto size = sessionList.size();

    if(size >= GetInstanceConfig().getNumber(NumberParamKey::USER_MAX_ALLOWED_SESSIONS)) // Deny if too many
    {
        return false;
    }

    Session session{metaData};
    token = session.getToken();

    sessionList.push_back(std::move(session));
    return true;
}

bool SessionStorage::get(const SessionToken& token, const SessionMetaData& metaData, UserID& user)
{
    UserSessionData* sessionData = getUserSessionData(user);
    if(sessionData == nullptr)
    {
        return false;
    }

    auto sessionList = sessionData->sessions.get();
    for(auto& session : sessionList)
    {
        if(session.getToken() == token) // Found a match
        {
            if(session.isValid(metaData))
            {
                user = sessionData->user;
                return true;
            }
            else                        // Metadata does not match
            {
                sessionList.erase(session);
                return false;
            }
        }
    }
    return false;
}

UserSessionData* SessionStorage::getUserSessionData(const UserID userID)
{
    for(auto& session : sessions)
    {
        if(session.user == userID)
        {
            return &session;
        }
    }
    return nullptr;
}


} // namespace tpunkt