#ifndef TPUNKT_SESSION_STORAGE_H
#define TPUNKT_SESSION_STORAGE_H

#include <vector>
#include "fwd.h"
#include "datastructures/FixedString.h"
#include "datastructures/SecureList.h"
#include "auth/Session.h"

namespace tpunkt
{
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