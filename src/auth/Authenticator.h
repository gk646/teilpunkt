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
    ERR_NO_ADMIN,          // Action requires admin but actor is not
};

const char* GetAuthStatusStr(AuthStatus status);

// Every method is atomic
struct Authenticator final
{
    //===== User Management =====//

    // Existing user adds a new user
    AuthStatus userAdd(UserID actor, const UserName& name, Credentials& consumed);

    // New user added via endpoint
    AuthStatus userAddServer(const UserName& name, Credentials& consumed);

    // Assigns token on success
    AuthStatus userLogin(const UserName& name, Credentials& consumed, UserID& user);

    // Deletes the given user and all their associated sessions
    AuthStatus userRemove(UserID actor, UserID user);

    // Changes the given users credentials to the given new name and credentials
    AuthStatus userChangeCredentials(UserID actor, const UserName& newName, Credentials& consumed);

    //===== Session Management =====//

    // Adds a new session with the given data to the user or reuses an existing one that matches
    AuthStatus sessionAdd(UserID actor, const SessionMetaData& metaData, SecureWrapper<SessionToken>& out);

    // Removes the session with the given creation timestamp
    AuthStatus sessionRemove(UserID actor, const Timestamp& creation);

    // Assigns user on success - only searches the given lookup user
    AuthStatus sessionAuth(UserID lookup, const SessionToken& token, const SessionMetaData& metaData, UserID& user);

    // Collects info of all session from the given user
    AuthStatus sessionGetInfo(UserID actor, std::vector<DTOSessionInfo>& collector);

    //===== User Data =====//
    // Part of the authenticator as data access needs the same atomicity as adding/deleting users

    // Assigns the username on success
    AuthStatus getUserName(UserID user, UserName& out);

    // Returns OK if given user is admin
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