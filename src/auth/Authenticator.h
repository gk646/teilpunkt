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
    AuthStatus userAdd(const UserName& name, Credentials& consumed);

    // Assigns token on success
    AuthStatus userLogin(const UserName& name, Credentials& consumed, UserID& user);

    // Deletes the given user and all their associated sessions
    AuthStatus userRemove(UserID actor, UserID user);

    // Changes the given users credentials to the given new name and credentials
    AuthStatus userChangeCredentials(UserID user, const UserName& newName, Credentials& consumed);

    //===== Session Management =====//

    // Adds a new session with the given data to the user
    AuthStatus sessionAdd(UserID user, const SessionMetaData& data, SecureWrapper<SessionToken>& out);

    // Removes the session with the given creation timestamp
    AuthStatus sessionRemove(UserID user, const Timestamp& creation);

    // Assigns user on success
    AuthStatus sessionAuth(const SessionToken& token, const SessionMetaData& data, UserID& user);

    AuthStatus sessionGetInfo(UserID user, std::vector<DTOSessionInfo>& collector);

    //===== User Data =====//
    // Part of the authenticator as data access needs the same atomicity as adding/deleting users

    // Assigns the username on success
    AuthStatus getUserName(UserID user, UserName& out);

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