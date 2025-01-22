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
        // all Methods that accept a token
        ERR_INVALID_TOKEN,
    };

    // Every method is atomic
    struct Authenticator final
    {
        //===== User Management =====//

        // Username must be unique
        AuthStatus userAdd(const UserName& name, Credentials& consumed);

        // Assigns token on success
        AuthStatus userLogin(const UserName& name, Credentials& consumed, AuthToken& out);

        // Deletes all associated sessions
        AuthStatus userRemove(const AuthToken& token);

        AuthStatus userChangeCredentials(const AuthToken& token, const UserName& newName, Credentials& consumed);

        //===== Session Management =====//

        // Assigns SessionID on success
        AuthStatus sessionAdd(const AuthToken& token, const SessionMetaData& data, SecureWrapper<SessionID>& out);

        AuthStatus sessionRemove(const AuthToken& token);

        // Assigns token on success
        AuthStatus sessionAuth(const SessionID& sessionId, const SessionMetaData& data, AuthToken& out);

        AuthStatus sessionGet(const AuthToken& token);

        //===== Token Management =====//

        [[nodiscard]] bool tokenValid(const AuthToken& token) const;

        AuthStatus tokenInvalidate(AuthToken& consumed);

        //===== User Data =====//
        // Part of the authenticator as data access needs the same atomicity as adding/deleting users

        // Assigns the username on success
        AuthStatus getUserName(const AuthToken& token, UserName& out);

        // Assigns the users wrapped key for the given file on success
        AuthStatus getWrappedKey(FileHandle handle, SecureWrapper<WrappedKey>& out);

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