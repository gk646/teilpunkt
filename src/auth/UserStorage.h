// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_USER_STORAGE_H
#define TPUNKT_USER_STORAGE_H

#include <vector>
#include "common/User.h"
#include "datastructures/SecureBox.h"
#include "datastructures/SecureWrapper.h"

namespace tpunkt
{

// Contains primitives to access user data - every function does only what it advertises
// Returns only a simple status code - true on success - false on failure
struct UserStorage final
{

    // Returns true if a new user with is added - name must be unique
    bool add(const UserName& name, const Credentials& credentials);

    // Returns true if the user with the given id was removed
    bool remove(UserID user);

    // Returns true and assigns user if a saved user matches the name and credentials
    bool login(const UserName& name, const Credentials& credentials, UserID& user) const;

    // Returns true if the users data is successfully changed
    [[nodiscard]] bool changeCredentials(UserID user, const UserName& newName, const Credentials& newCredentials);

    // Returns true if a user with the given name exists
    [[nodiscard]] bool nameExists(const UserName& name) const;

    [[nodiscard]] bool getName(UserID user, UserName& data) const;

  private:
    UserStorage();
    ~UserStorage();

    std::vector<SecureBox<User>> users;
    SecureWrapper<uint32_t> currUserID; // Chosen randomly once - then incremented for each new user
    TPUNKT_MACROS_STRUCT(UserStorage);
    friend Authenticator;
};

} // namespace tpunkt


#endif // TPUNKT_USER_STORAGE_H
