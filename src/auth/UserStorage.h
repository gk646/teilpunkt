#ifndef TPUNKT_USER_STORAGE_H
#define TPUNKT_USER_STORAGE_H

#include <vector>
#include "auth/Credentials.h"
#include "common/User.h"

namespace tpunkt
{
    enum class UserStorageStatus : uint8_t
    {
        USER_NAME_EXISTS,

    };

    struct UserStorage final
    {
        std::vector<User> users;

        bool addUser(const char* name, const char* secret, CredentialsType type);
    };

} // namespace tpunkt


#endif //TPUNKT_USER_STORAGE_H