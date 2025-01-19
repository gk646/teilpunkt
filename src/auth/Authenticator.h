#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include "fwd.h"
#include "util/Macros.h"
#include "auth/SessionStorage.h"
#include "auth/UserStorage.h"
#include "datastructures/SecureWrapper.h"

namespace tpunkt
{
    enum class AuthStatus : uint8_t
    {
        INVALID, // Prevent default initialized values
        OK,
        // Generic error
        ERR_UNSUCCESSFUL,
        // userAdd
        ERR_USER_NAME_EXISTS,
    };

    struct Authenticator final
    {
        //===== User Management =====//

        // Logs the user in and assigns token on success
        AuthStatus userLogin(const UserName& name, Credentials& consumed, AuthToken& out);

        // Adds a new user - username must be unique
        AuthStatus userAdd(const UserName& name, Credentials& consumed);

        // Removes the user and all their sessions
        AuthStatus userRemove(const AuthToken& token);

        // Changes the users credentials
        AuthStatus userChangeCredentials(const AuthToken& token, const UserName& newName, Credentials& consumed);

        //===== Session Management =====//

        // Adds a session and returns the session id
        AuthStatus sessionAdd(const AuthToken& token, const SessionData& data, SecureWrapper<SessionID>& out);

        // Remove the session associated with the token
        AuthStatus sessionRemove(const AuthToken& token);

        // Tries to authenticate the user via the session id
        AuthStatus sessionAuth(const SessionID& sessionId, const SessionData& data, AuthToken& out);

        // Get all session for the user associated with the token
        AuthStatus sessionGet(const AuthToken& token);

        //===== Token Management =====//

        // Returns true if the given auth token is valid
        bool tokenValid(const AuthToken& token);

        // Invalidates the given token - zeroes it and deletes it internally
        AuthStatus tokenInvalidate(AuthToken& token);

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