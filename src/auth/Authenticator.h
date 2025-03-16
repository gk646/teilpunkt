// SPDX-License-Identifier: GPL-3.0-only

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
    INVALID,               // Prevent default initialized values
    OK,
    ERR_UNSUCCESSFUL,      // Generic error
    ERR_USER_NAME_EXISTS,  // userAdd
    ERR_INVALID_TOKEN,     // all Methods that accept a token
    ERR_INVALID_ARGUMENTS, // One or more arguments are invalid
};

const char* GetAuthStatusStr(AuthStatus status);

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
    AuthStatus userRemove(UserID user);

    AuthStatus userChangeCredentials(UserID user, const UserName& newName, Credentials& consumed);

    //===== Session Management =====//

    AuthStatus sessionAdd(UserID user, const SessionMetaData& data, SecureWrapper<SessionID>& out);

    AuthStatus sessionRemove(UserID user, int idx);

    // Assigns user on success
    AuthStatus sessionAuth(const SessionID& sessionId, const SessionMetaData& data, UserID& user);

    // TODO make collector class / vector like container with stack memory / wrapper around c pointer with len
    AuthStatus sessionGet(const AuthToken& token);

    //===== User Data =====//
    // Part of the authenticator as data access needs the same atomicity as adding/deleting users

    // Assigns the username on success
    AuthStatus getUserName(const AuthToken& token, UserName& out);

    // Assigns the users wrapped key for the given file on success
    AuthStatus getWrappedKey(const AuthToken& token, FileID file, SecureWrapper<WrappedKey>& out);

    // Returns OK if true
    AuthStatus getIsAdmin(UserID user);

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

} // namespace tpunkt

#endif // TPUNKT_AUTHENTICATOR_H