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
        explicit UserSessionData(const SecureBox<User>* user) : userBox(user)
        {
        }

        // Only allow move construction
        UserSessionData(UserSessionData&& other) noexcept : userBox(other.userBox)
        {
            sessions = std::move(other.sessions);
            tokens = std::move(other.tokens);
            other.userBox = nullptr;
        }

        UserSessionData(const UserSessionData&) = delete;
        UserSessionData& operator=(const UserSessionData&) = delete;
        UserSessionData& operator=(UserSessionData&& other) = delete;

      private:
        const SecureBox<User>* userBox = nullptr; // Who these sessions belong to
        SecureList<Session> sessions;             // Session list
        std::vector<uint32_t> tokens;             // Token list
        friend SessionStorage;
    };

    struct SessionStorage final
    {
        SessionStorage() = default;

        bool add(const SecureBox<User>& user, const SessionMetaData& data, SessionID& out);
        bool removeByRemote(const SecureBox<User>& user, const HashedIP& address);
        bool removeByID(const SecureBox<User>& user, const SessionID& sessionId);

        //===== Token Management =====//

        [[nodiscard]] bool tokenValid(const AuthToken& token) const;
        bool addToken(const SecureBox<User>& user, uint32_t& out);
        bool removeToken(const SecureBox<User>& user, uint32_t random);

      private:
        UserSessionData* getUserData(const SecureBox<User>& user);
        [[nodiscard]] const UserSessionData* getUserData(const SecureBox<User>& user) const;

        std::vector<UserSessionData> sessions;
        TPUNKT_MACROS_STRUCT(SessionStorage);
    };

} // namespace tpunkt

#endif // TPUNKT_SESSION_STORAGE_H