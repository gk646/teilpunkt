// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "auth/Session.h"
#include "datastructures/SecureList.h"
#include "fwd.h"

namespace tpunkt
{
struct UserSessionData final
{
    explicit UserSessionData(const UserID user) : userID(user)
    {
    }

    // Only allow move construction
    UserSessionData(UserSessionData&& other) noexcept
        : userID(other.userID), sessions(std::move(other.sessions)), tokens(std::move(other.tokens))
    {
    }

    UserSessionData(const UserSessionData&) = delete;
    UserSessionData& operator=(const UserSessionData&) = delete;
    UserSessionData& operator=(UserSessionData&& other) = delete;

  private:
    const UserID userID;          // User it belongs to
    SecureList<Session> sessions; // Session list
    friend SessionStorage;
};

struct SessionStorage final
{
    SessionStorage() = default;

    //===== Session Management =====//

    bool add(UserID userID, const SessionMetaData& data, SessionID& out);

    // If the session is identified but the data does not match its revoked!
    bool get(const SessionID& sessionId, const SessionMetaData& data, UserID& userID);

    bool removeByRemote(UserID userID, const HashedIP& address);

    bool removeByID(UserID userID, const SessionID& sessionId);

    //===== Token Management =====//

    [[nodiscard]] bool tokenValid(const AuthToken& token) const;

    bool addToken(UserID userID, uint32_t& random);

    bool removeToken(const AuthToken& token);

  private:
    UserSessionData* getUserData(UserID userID);

    [[nodiscard]] const UserSessionData* getUserData(UserID userID) const;

    std::vector<UserSessionData> sessions;
    TPUNKT_MACROS_STRUCT(SessionStorage);
};

} // namespace tpunkt

#endif // TPUNKT_SESSION_STORAGE_H