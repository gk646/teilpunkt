#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "fwd.h"
#include "datastructures/FixedString.h"
#include "datastructures/SecureList.h"

namespace tpunkt
{

    // A session saves the authentication so the user does not have to authenticate on each request
    // A session is terminated if ONE of the following is true:
    //      - the current time is greater than the expiration stamp
    //      - the remote address with the cookie differs from the saved one
    //      - the user agent string differs from the saved one
    struct Session final
    {
        Session() = default;
        SessionID sessionID;
        UserAgentString userAgent;
        HashedIP remoteAddress;
        uint64_t expiration;
    };

    struct UserSessions final
    {
        const SecureBox<User>* userBox = nullptr; // Who these sessions belong to
        SecureList<Session> sessions;             // Session list
        std::vector<uint32_t> tokens;             // Tokens per user
    };

    struct SessionStorage final
    {
        SessionStorage() = default;

        SessionID add(const SecureBox<User>* user);
        bool removeByIP(const SecureBox<User>* user, HashedIP& address);
        bool removeByID(const SecureBox<User>* user, SessionID& address);

      private:
        std::vector<UserSessions> sessions;
        TPUNKT_MACROS_STRUCT(SessionStorage);
    };

} // namespace tpunkt

#endif // TPUNKT_SESSION_STORAGE_H