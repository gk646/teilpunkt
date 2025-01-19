#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include "fwd.h"
#include "util/Macros.h"
#include "auth/SessionStorage.h"
#include "auth/UserStorage.h"

namespace tpunkt
{

    enum class AuthenticatorStatus : uint8_t
    {
        INVALID,
        OK,
        // Generic error
        ERR_UNSUCCESSFUL,
        // userAdd
        ERR_USER_NAME_EXISTS,
    };

    struct Authenticator final
    {
        //===== User Management =====//

        // Tries to log in the user creating a new session if successful
        AuthenticatorStatus userLogin(const UserName& name, Credentials& credentials, AuthToken& out);

        // Deletes the currently used session
        AuthenticatorStatus userLogout(const AuthToken& token);

        // Adds a new user if the username is unique
        AuthenticatorStatus userAdd(const UserName& name, Credentials& credentials);

        // Removes the user and all their sessions
        AuthenticatorStatus removeUser(const AuthToken& token);

        // Changes the users credentials
        AuthenticatorStatus userChangeCredentials(const AuthToken& token, Credentials& newCredentials);

        //===== Session Management =====//

        AuthenticatorStatus sessionRemove(const AuthToken& token);

        // Tries to authenticate the user via the session id
        AuthenticatorStatus sessionAuth(const SessionID& sessionId, AuthToken& out);

        AuthenticatorStatus sessionGet();

        //===== Token Management =====//

        // Returns true if the given auth token is valid
        bool tokenValid(const AuthToken& token);

        AuthenticatorStatus tokenInvalidate(AuthToken& token);

        Authenticator();
        ~Authenticator();

      private:
        SessionStorage sessionStore;
        UserStorage userStore;
        TPUNKT_MACROS_STRUCT(Authenticator);
    };

    Authenticator& GetAuthenticator();

} // namespace tpunkt

#endif // TPUNKT_AUTHENTICATOR_H