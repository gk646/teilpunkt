#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include "fwd.h"
#include "util/Macros.h"

namespace tpunkt
{
    struct Authenticator final
    {
        // Tries to log in the user creating a new session if successful
        AuthToken loginUser(const UserName& name, const Credentials& credentials);

        // Tries to authenticate the user via the session id
        AuthToken authCookie(const SessionID& sessionId);

        // Returns true if the given auth token is valid
        bool isValid(const AuthToken& token);

        Authenticator();
        ~Authenticator();

    private:
        SecureList<SessionStorage>* sessionStore{};
        SecureList<UserStorage>* userStore{};
        TPUNKT_MACROS_STRUCT(Authenticator);
    };

    Authenticator& GetAuthenticator();

} // namespace tpunkt

#endif //TPUNKT_AUTHENTICATOR_H