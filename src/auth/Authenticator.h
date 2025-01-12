#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include "fwd.h"
#include "datastructures/SecureContainer.h"

namespace tpunkt
{
    struct Authenticator final
    {
        AuthToken loginUser(const UserName& name, const Credentials& credentials);
        AuthToken authCookie(const SessionID& sessionId);

        bool isValid(const AuthToken& token);

    private:

        SecureContainer<SessionStorage> sessionStore;
        UserStorage* userStore;
    };

    Authenticator& GetAuthenticator();
} // namespace tpunkt

#endif //TPUNKT_AUTHENTICATOR_H