// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include "auth/SessionStorage.h"
#include "auth/UserStorage.h"
#include "datastructures/Spinlock.h"
#include "fwd.h"
#include "util/Macros.h"

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
// Admin can only Add
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

    // TODO needs additional parameter by which to identify session
    AuthStatus sessionRemove(const AuthToken& token);

    // Assigns token on success
    AuthStatus sessionAuth(const SessionID& sessionId, const SessionMetaData& data, AuthToken& out);

    // TODO make collector class / vector like container with stack memory / wrapper around c pointer with len
    AuthStatus sessionGet(const AuthToken& token);

    //===== Token Management =====//

    [[nodiscard]] bool tokenValid(const AuthToken& token);

    AuthStatus tokenInvalidate(AuthToken& consumed);

    //===== User Data =====//
    // Part of the authenticator as data access needs the same atomicity as adding/deleting users

    // Assigns the username on success
    AuthStatus getUserName(const AuthToken& token, UserName& out);

    // Assigns the users wrapped key for the given file on success
    AuthStatus getWrappedKey(const AuthToken& token, FileID file, SecureWrapper<WrappedKey>& out);

    // Returns OK if true
    AuthStatus getIsAdmin(const AuthToken& token, UserID user);

    //===== Admin  =====//

    Authenticator();
    ~Authenticator();

  private:
    Spinlock authLock;           // Makes all operations atomic
    SessionStorage sessionStore; // Saves all sessions and tokens
    UserStorage userStore;       // Saves all userdata
    TPUNKT_MACROS_STRUCT(Authenticator);
};

Authenticator& GetAuthenticator();

#define TPUNKT_NO_AUTH_RET(token, retval)                                                                              \
    if(!GetAuthenticator().tokenValid(token))                                                                          \
    {                                                                                                                  \
        return retval;                                                                                                 \
    }


#define TPUNKT_NO_AUTH_RET_ADD(token, retval, add)                                                                     \
    if(!GetAuthenticator().tokenValid(token))                                                                          \
    {                                                                                                                  \
        add;                                                                                                           \
        return retval;                                                                                                 \
    }

} // namespace tpunkt

#endif // TPUNKT_AUTHENTICATOR_H