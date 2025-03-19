// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "auth/Session.h"
#include "datastructures/SecureList.h"
#include "fwd.h"

namespace tpunkt
{

// Groups session data per user
struct UserSessionData final
{
    explicit UserSessionData(const UserID user) : user(user)
    {
    }

    // Only allow move construction
    UserSessionData(UserSessionData&& other) noexcept : user(other.user), sessions(std::move(other.sessions))
    {
    }

    UserSessionData(const UserSessionData&) = delete;
    UserSessionData& operator=(const UserSessionData&) = delete;
    UserSessionData& operator=(UserSessionData&& other) = delete;

  private:
    const UserID user;            // User it belongs to
    SecureList<Session> sessions; // Session list
    friend SessionStorage;
};

// Stores and manages all session data
struct SessionStorage final
{
    SessionStorage() = default;

    //===== Session Management =====//

    // Returns true if a new session with the give meta-data was added and token is set
    bool add(UserID userID, const SessionMetaData& data, SessionToken& token);

    // Returns true a valid session was found - automatically revokes if the token matches but not the meta-data
    bool get(const SessionToken& token, const SessionMetaData& data, UserID& userID);

    // Returns true if the session of the user with the given uid was removed
    bool removeByUID(UserID userID, int uid);

  private:
    UserSessionData* getUserSessionData(UserID userID);
    [[nodiscard]] const UserSessionData* getUserSessionData(UserID userID) const;

    std::vector<UserSessionData> sessions;
    TPUNKT_MACROS_STRUCT(SessionStorage);
};

} // namespace tpunkt

#endif // TPUNKT_SESSION_STORAGE_H