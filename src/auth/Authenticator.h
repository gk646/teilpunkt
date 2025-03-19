// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_AUTHENTICATOR_H
#define TPUNKT_AUTHENTICATOR_H

#include <vector>
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
    ERR_INVALID_ARGUMENTS, // One or more arguments are invalid
};

const char* GetAuthStatusStr(AuthStatus status);

// Every method is atomic
struct Authenticator final
{
    //===== User Management =====//

    AuthStatus userAdd(UserID user, const UserName& name, Credentials& consumed);

    // Assigns token on success
    AuthStatus userLogin(const UserName& name, Credentials& consumed, UserID& out);

    // Deletes all associated sessions
    AuthStatus userRemove(UserID user);

    AuthStatus userChangeCredentials(UserID user, const UserName& newName, Credentials& consumed);

    //===== Session Management =====//

    AuthStatus sessionAdd(UserID user, const SessionMetaData& data, SecureWrapper<SessionToken>& out);

    AuthStatus sessionRemove(UserID user, int idx);

    // Assigns user on success
    AuthStatus sessionAuth(const SessionToken& token, const SessionMetaData& data, UserID& out);

    // TODO make collector class / vector like container with stack memory / wrapper around c pointer with len
    AuthStatus sessionGet(UserID user, std::vector<>);

    //===== User Data =====//
    // Part of the authenticator as data access needs the same atomicity as adding/deleting users

    // Assigns the username on success
    AuthStatus getUserName(UserID user, UserName& out);

    // Assigns the users wrapped key for the given file on success
    AuthStatus getWrappedKey(UserID user, FileID file, SecureWrapper<WrappedKey>& out);

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