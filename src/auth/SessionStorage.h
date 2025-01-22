#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "fwd.h"
#include "datastructures/SecureList.h"
#include "auth/Session.h"

namespace tpunkt
{
    struct UserSessionData final
    {
        explicit UserSessionData(const uint32_t user) : userID(user)
        {
        }

        // Only allow move construction
        UserSessionData(UserSessionData&& other) noexcept : userID(other.userID)
        {
            sessions = std::move(other.sessions);
            tokens = std::move(other.tokens);
        }

        UserSessionData(const UserSessionData&) = delete;
        UserSessionData& operator=(const UserSessionData&) = delete;
        UserSessionData& operator=(UserSessionData&& other) = delete;

      private:
        const uint32_t userID;        // Which users session data
        SecureList<Session> sessions; // Session list
        std::vector<uint32_t> tokens; // Token list
        friend SessionStorage;
    };

    struct SessionStorage final
    {
        SessionStorage() = default;

        //===== Session Management =====//

        bool add(uint32_t userID, const SessionMetaData& data, SessionID& out);

        bool removeByRemote(uint32_t userID, const HashedIP& address);

        bool removeByID(uint32_t userID, const SessionID& sessionId);

        //===== Token Management =====//

        [[nodiscard]] bool tokenValid(const AuthToken& token) const;

        bool addToken(uint32_t userID, uint32_t& random);

        bool removeToken(const AuthToken& token);

      private:
        UserSessionData* getUserData(uint32_t userID);

        [[nodiscard]] const UserSessionData* getUserData(uint32_t userID) const;

        std::vector<UserSessionData> sessions;
        TPUNKT_MACROS_STRUCT(SessionStorage);
    };

} // namespace tpunkt

#endif // TPUNKT_SESSION_STORAGE_H