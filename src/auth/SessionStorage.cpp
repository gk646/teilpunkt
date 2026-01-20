// SPDX-License-Identifier: GPL-3.0-only
#include <sodium/randombytes.h>
#include "auth/SessionStorage.h"
#include "instance/InstanceConfig.h"
#include "server/DTO.h"

namespace tpunkt
{

Session::Session(const SessionMetaData& metaData)
    : metaData(metaData),
      expiration(Timestamp::Now(GetInstanceConfig().getNumber(NumberParamKey::USER_SESSION_EXPIRATION_SECS)))
{
    randombytes_buf(token.data(), token.capacity());
    sodium_bin2base64(token.data(), token.capacity(), token.udata(), TPUNKT_CRYPTO_SESSION_ID_LEN,
                      sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
}

Session::Session(Session&& other) noexcept
    : token(other.token), metaData(other.metaData), creation(other.creation), expiration(other.expiration)
{
}

bool Session::isValid(const SessionMetaData& newMetaData) const
{
    if(metaData != newMetaData)
    {
        return false;
    }
    if(expiration.isInPast())
    {
        return false;
    }
    return true;
}

const SessionToken& Session::getToken() const
{
    return token;
}

const Timestamp& Session::getCreation() const
{
    return creation;
}

const Timestamp& Session::getExpiration() const
{
    return expiration;
}

const UserAgentString& Session::getUserAgent() const
{
    return metaData.userAgent;
}

bool Session::isExpired() const
{
    return expiration.isInPast();
}

UserSessionData::UserSessionData(const UserID user) : user(user)
{
}

UserID UserSessionData::getUser() const
{
    return user;
}

SecureList<Session>& UserSessionData::getSessions()
{
    return sessions;
}

bool SessionStorage::add(const UserID user, const SessionMetaData& metaData, SessionToken& token)
{
    UserSessionData* userData = getUserSessionData(user);
    if(userData == nullptr) // User is not present yet
    {
        userData = &sessions.emplace_back(user);
    }

    auto sessionList = userData->getSessions().get();

    // Evict expired session on each adds
    sessionList.erase_if([](const Session& session) { return session.isExpired(); });

    // Check if session exists for current metadata
    for(auto& session : sessionList)
    {
        if(session.isValid(metaData))
        {
            token = session.getToken();
            return true;
        }
    }

    // Deny if too many
    if(sessionList.size() >= GetInstanceConfig().getNumber(NumberParamKey::USER_MAX_ALLOWED_SESSIONS))
    {
        return false;
    }


    Session session{metaData};
    token = session.getToken();
    sessionList.push_back(std::move(session));

    return true;
}

bool SessionStorage::get(const UserID lookup, const SessionToken& token, const SessionMetaData& metaData, UserID& user)
{
    UserSessionData* userData = getUserSessionData(lookup);
    if(userData == nullptr) // User is not present yet
    {
        return false;
    }

    if(userData->getUser() != lookup)
    {
        return false;
    }

    auto sessionList = userData->getSessions().get();
    for(size_t i = 0; i < sessionList.size(); ++i)
    {
        auto& session = sessionList[ i ];
        if(session.getToken() == token) // Found a match
        {
            if(session.isValid(metaData))
            {
                user = userData->getUser();
                return true;
            }
            // Metadata does not match
            sessionList.eraseIndex(i);
            return false;
        }
    }
    return false;
}

bool SessionStorage::remove(const UserID user, const Timestamp& creation)
{
    UserSessionData* sessionData = getUserSessionData(user);
    if(sessionData == nullptr) [[unlikely]]
    {
        return false;
    }
    auto sessionList = sessionData->getSessions().get();
    for(size_t i = 0; i < sessionList.size(); ++i)
    {
        auto& session = sessionList[ i ];
        if(session.getCreation() == creation)
        {
            sessionList.eraseIndex(i);
            return true;
        }
    }
    return false;
}

void SessionStorage::getInfo(const UserID user, std::vector<DTO::SessionInfo>& collector)
{
    collector.clear();
    UserSessionData* sessionData = getUserSessionData(user);
    if(sessionData == nullptr) [[unlikely]]
    {
        return;
    }
    auto sessionList = sessionData->getSessions().get();
    for(auto& session : sessionList)
    {
        DTO::SessionInfo info;
        info.creationUnix = session.getCreation().getNanos();
        info.expirationUnix = session.getExpiration().getNanos();
        info.userAgent = session.getUserAgent();
        collector.push_back(info);
    }
}

UserSessionData* SessionStorage::getUserSessionData(const UserID userID)
{
    for(auto& session : sessions)
    {
        if(session.getUser() == userID)
        {
            return &session;
        }
    }
    return nullptr;
}

} // namespace tpunkt
