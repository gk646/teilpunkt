#ifndef TPUNKT_USER_STORAGE_H
#define TPUNKT_USER_STORAGE_H

#include "auth/Credentials.h"
#include "common/User.h"
#include "datastructures/SecureList.h"

namespace tpunkt
{

    struct UserStorage final
    {
        bool addUser(const UserName& name, const Credentials& credentials);
        bool loginUser(const UserName& name, const Credentials& credentials, User&);
        bool contains(const UserName& name);

        UserStorage() = default;

      private:
        SecureList<User> users;
        TPUNKT_MACROS_STRUCT(UserStorage);
    };

} // namespace tpunkt


#endif // TPUNKT_USER_STORAGE_H