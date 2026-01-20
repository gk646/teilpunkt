// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "datastructures/FixedString.h"
#include "datastructures/SecureList.h"
#include "datastructures/Timestamp.h"

namespace tpunkt
{

// Data associated with each session - has to match
struct SessionMetaData final
{
    UserAgentString userAgent;
    HashedIP remoteAddress;

    bool operator==(const SessionMetaData&) const = default;
};

// A session saves the authentication so the user does not have to authenticate on each request
// A session is terminated if ONE of the following is true:
//      - the current time is greater than the expiration stamp
//      - the remote address with the cookie differs from the saved one
//      - the user agent string differs from the saved one
struct Session final
{
    Session() = default;
    explicit Session(const SessionMetaData& metaData);
    Session(Session&& other) noexcept;

    [[nodiscard]] bool isValid(const SessionMetaData& metaData) const;

    [[nodiscard]] const SessionToken& getToken() const;

    [[nodiscard]] const Timestamp& getCreation() const;

    [[nodiscard]] const Timestamp& getExpiration() const;

    [[nodiscard]] const UserAgentString& getUserAgent() const;

    [[nodiscard]] bool isExpired() const;

  private:
    SessionToken token;       // Actual set cookie
    SessionMetaData metaData; // Metadata that has to match
    const Timestamp creation; // Tracked for user info
    const Timestamp expiration;
};

// Groups session data per user
struct UserSessionData final
{
    explicit UserSessionData(UserID user);
    TPUNKT_MACROS_MOVE_ONLY(UserSessionData);

    UserID getUser() const;

    SecureList<Session>& getSessions();

  private:
    const UserID user;            // User it belongs to
    SecureList<Session> sessions; // Session list
};

// Stores and manages all session data
struct SessionStorage final
{
    // Returns true if either a new session was created or an existing reused and token is set to the session token
    bool add(UserID user, const SessionMetaData& metaData, SessionToken& token);

    // Returns true a valid session was found - automatically revokes if the token matches but not the meta-data
    bool get(UserID lookup, const SessionToken& token, const SessionMetaData& metaData, UserID& user);

    // Returns true if the session of the user with the given timestamp was removed
    bool remove(UserID user, const Timestamp& creation);

    void getInfo(UserID user, std::vector<DTO::SessionInfo>& collector);

  private:
    SessionStorage() = default;

    UserSessionData* getUserSessionData(UserID userID);

    std::vector<UserSessionData> sessions;
    TPUNKT_MACROS_STRUCT(SessionStorage);
    friend Authenticator;
};

} // namespace tpunkt

#endif // TPUNKT_SESSION_STORAGE_H
