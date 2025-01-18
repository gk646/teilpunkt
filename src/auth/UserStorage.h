#ifndef TPUNKT_USER_STORAGE_H
#define TPUNKT_USER_STORAGE_H

#include <vector>
#include "auth/Credentials.h"
#include "common/User.h"
#include "datastructures/SecureBox.h"

namespace tpunkt
{
    // Contains primitives to access user data - every function does only what it advertises
    // Returns only a simple status code - true on success - false on failure
    struct UserStorage final
    {
        bool add(const UserName& name, const Credentials& credentials);
        bool remove(const UserName& name);
        bool login(const UserName& name, const Credentials& credentials, const SecureBox<User>*& user);
        bool changeName();
        bool changeCredentials();

        [[nodiscard]] bool nameExists(const UserName& name) const;

        UserStorage();
        ~UserStorage();

      private:
        std::vector<SecureBox<User>> users;
        TPUNKT_MACROS_STRUCT(UserStorage);
    };

} // namespace tpunkt


#endif // TPUNKT_USER_STORAGE_H