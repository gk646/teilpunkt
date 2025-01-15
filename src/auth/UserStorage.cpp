#include "auth/UserStorage.h"
#include "util/FileIO.h"

namespace tpunkt
{
    bool UserStorage::addUser(const UserName& name, const Credentials& credentials)
    {
        return true;
    }
    bool UserStorage::loginUser(const UserName& name, const Credentials& credentials, User&)
    {
        return true;
    }
    bool UserStorage::contains(const UserName& name)
    {
        return false;
    }
} // namespace tpunkt