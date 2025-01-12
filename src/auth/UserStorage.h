#ifndef TPUNKT_USER_STORAGE_H
#define TPUNKT_USER_STORAGE_H

#include <vector>
#include "auth/Credentials.h"
#include "common/User.h"
#include "util/Macros.h"

namespace tpunkt
{
    enum class UserStorageStatus : uint8_t
    {
        USER_NAME_EXISTS,
        INVALID_CREDENTIALS,
    };

    struct UserStorage final
    {
        std::vector<User> users;

        bool addUser(const UserName& name, const char* secret, CredentialsType type);

        TPUNKT_MACROS_STRUCT(UserStorage);
    };

} // namespace tpunkt


#endif //TPUNKT_USER_STORAGE_H