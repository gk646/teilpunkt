// SPDX-License-Identifier: GPL-3.0-only

#include <sodium/randombytes.h>
#include "auth/SessionStorage.h"
#include "instance/InstanceConfig.h"

namespace tpunkt
{

bool SessionStorage::add(const UserID userID, const SessionMetaData& data, SessionToken& token)
{
    auto* userData = getUserSessionData(userID);
    if(userData == nullptr)
    {
        userData = &sessions.emplace_back(userID);
    }

    auto sessionList = userData->sessions.get();
    const auto size = sessionList.size();

    if(size >= GetInstanceConfig().getNumber(NumberParamKey::USER_MAX_ALLOWED_SESSIONS)) // Deny if too many
    {
        return false;
    }

    Session session{};
    session.data = data;
    session.expiration.addMins(GetInstanceConfig().getNumber(NumberParamKey::USER_SESSION_EXPIRATION_DELAY_SECS));

    randombytes_buf(session.token.data(), session.token.capacity());
    token = session.token;

    sessionList.push_back(std::move(session));
    return true;
}

bool SessionStorage::get(const SessionToken& token, const SessionMetaData& data, UserID& userID)
{
    return true;
}

bool SessionStorage::removeByUID(const UserID userID, const HashedIP& address)
{
    auto* userData = getUserSessionData(userID);
    if(userData == nullptr)
    {
        return false;
    }
    auto sessionList = userData->sessions.get();
    for(size_t i = 0U; i < sessionList.size(); ++i)
    {
        if(sessionList[ i ].data.remoteAddress == address)
        {
            return sessionList.eraseIndex(i);
        }
    }
    return false;
}

bool SessionStorage::removeByID(const UserID userID, const SessionToken& sessionId)
{
    auto* userData = getUserSessionData(userID);
    if(userData == nullptr)
    {
        return false;
    }
    auto sessionList = userData->sessions.get();
    for(size_t i = 0U; i < sessionList.size(); ++i)
    {
        if(sessionList[ i ].token == sessionId)
        {
            return sessionList.eraseIndex(i);
        }
    }
    return false;
}

bool SessionStorage::tokenValid(const AuthToken& token) const
{
    if(token.userID == UserID::INVALID)
    {
        return false; // Invalid userID
    }

    const auto* userData = getUserSessionData(token.userID);
    if(userData == nullptr)
    {
        return false;
    }

    for(const auto random : userData->tokens)
    {
        if(random == token.random)
        {
            return true;
        }
    }

    return false;
}

bool SessionStorage::addToken(const UserID userID, uint32_t& random)
{
    auto* userData = getUserSessionData(userID);
    if(userData == nullptr)
    {
        userData = &sessions.emplace_back(userID);
    }

    random = randombytes_random();
    userData->tokens.push_back(random);
    return true;
}

bool SessionStorage::removeToken(const AuthToken& token)
{
    auto* userData = getUserSessionData(token.userID);
    if(userData == nullptr)
    {
        return true; // Token is invalid as user doesnt exist
    }

    for(auto& savedRandom : userData->tokens)
    {
        if(savedRandom == token.random)
        {
            savedRandom = userData->tokens.back();
            userData->tokens.pop_back();
            return true;
        }
    }
    return true; // Token could not be found - not valid anymore
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

const UserSessionData* SessionStorage::getUserSessionData(const UserID userID) const
{
    for(const auto& session : sessions)
    {
        if(session.user == userID)
        {
            return &session;
        }
    }
    return nullptr;
}

} // namespace tpunkt