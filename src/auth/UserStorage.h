// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_USER_STORAGE_H
#define TPUNKT_USER_STORAGE_H

#include <vector>
#include "auth/Credentials.h"
#include "common/User.h"
#include "datastructures/SecureBox.h"
#include "datastructures/SecureWrapper.h"

namespace tpunkt
{
    // Contains primitives to access user data - every function does only what it advertises
    // Returns only a simple status code - true on success - false on failure
    struct UserStorage final
    {
        bool add(const UserName& name, const Credentials& credentials);

        bool remove(UserID user);

        bool login(const UserName& name, const Credentials& credentials, UserID& userID) const;

        [[nodiscard]] bool changeCredentials(UserID use, const UserName& newName,
                                             const Credentials& newCredentials);

        [[nodiscard]] bool nameExists(const UserName& name) const;

        UserStorage();
        ~UserStorage();

      private:
        std::vector<SecureBox<User>> users;
        SecureWrapper<std::underlying_type_t<UserID>> currUserID; // At first launch chosen randomly - then incremented for each new user
        TPUNKT_MACROS_STRUCT(UserStorage);
    };

} // namespace tpunkt


#endif // TPUNKT_USER_STORAGE_H