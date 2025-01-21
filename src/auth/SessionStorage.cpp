#include <sodium/randombytes.h>
#include "auth/SessionStorage.h"
#include "auth/AuthToken.h"

namespace tpunkt
{
    bool SessionStorage::add(const SecureBox<User>& user, const SessionMetaData& data, SessionID& out)
    {
        auto* userData = getUserData(user);
        if(userData == nullptr)
        {
            userData = &sessions.emplace_back(&user);
        }

        auto sessionList = userData->sessions.get();
        const auto size = sessionList.size();

        if(size >= 10U)                  // Deny if too many
        {
            return false;
        }

        Session session{};
        session.data = data;
        session.expiration.addDays(5U); // Get config value

        randombytes_buf(session.sessionID.data(), session.sessionID.capacity());
        out = session.sessionID;

        sessionList.push_back(std::move(session));
        return true;
    }

    bool SessionStorage::removeByRemote(const SecureBox<User>& user, const HashedIP& address)
    {
        auto* userData = getUserData(user);
        if(userData == nullptr)
        {
            return false;
        }
        auto sessionList = userData->sessions.get();
        for(size_t i = 0U; i < sessionList.size(); ++i)
        {
            if(sessionList[ i ].data.remoteAddress == address)
            {
                sessionList.eraseIndex(i);
                return true;
            }
        }
        return false;
    }

    bool SessionStorage::removeByID(const SecureBox<User>& user, const SessionID& sessionId)
    {
        auto* userData = getUserData(user);
        if(userData == nullptr)
        {
            return false;
        }
        auto sessionList = userData->sessions.get();
        for(size_t i = 0U; i < sessionList.size(); ++i)
        {
            if(sessionList[ i ].sessionID == sessionId)
            {
                sessionList.eraseIndex(i);
                return true;
            }
        }
        return false;
    }

    bool SessionStorage::tokenValid(const AuthToken& token) const
    {
        const auto* userData = getUserData(token.getUserBox());
        if(userData == nullptr)
        {
            return false;
        }
        else
        {
            for(const auto random : userData->tokens)
            {
                if(random == token.random)
                {
                    return true;
                }
            }
        }
    }

    bool SessionStorage::addToken(uint32_t& random)
    {

    }

    bool SessionStorage::removeToken(uint32_t random)
    {

    }

    UserSessionData* SessionStorage::getUserData(const SecureBox<User>& user)
    {
        for(auto& session : sessions)
        {
            if(session.userBox == &user)
            {
                return &session;
            }
        }
        return nullptr;
    }

    const UserSessionData* SessionStorage::getUserData(const SecureBox<User>& user) const
    {
        for(auto& session : sessions)
        {
            if(session.userBox == &user)
            {
                return &session;
            }
        }
        return nullptr;
    }

} // namespace tpunkt