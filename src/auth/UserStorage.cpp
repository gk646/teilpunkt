#include "auth/UserStorage.h"
#include "util/FileIO.h"

namespace tpunkt
{
    UserStorage::UserStorage()
    {
    }
    UserStorage::~UserStorage()
    {
    }

    bool UserStorage::addUser(const UserName& name, const Credentials& credentials)
    {
        User newUser;
        newUser.name = name;

        switch(credentials.type)
        {
            case CredentialsType::INVALID:
                return false;
            case CredentialsType::PASSWORD:
                {
                    char* storePw = newUser.credentials.password.data();
                    const char* newPw = credentials.password.c_str();
                    const auto newPwLen = credentials.password.size();
                    constexpr auto calcDiff = crypto_pwhash_OPSLIMIT_INTERACTIVE;
                    constexpr auto memoryDiff = crypto_pwhash_MEMLIMIT_INTERACTIVE;
                    if(crypto_pwhash_str(storePw, newPw, newPwLen, calcDiff, memoryDiff) != 0)
                    {
                        return false; // Out of memory
                    }
                }
                break;
            case CredentialsType::PASSKEY:
                break;
        }

        auto list = users.get();
        list.push_back(newUser);

        return true;
    }

    bool UserStorage::loginUser(const UserName& name, const Credentials& credentials, User& loginUser)
    {
        auto list = users.get();
        for(const auto& user : list)
        {
            if(user.name == name) [[unlikely]]
            {
                switch(credentials.type)
                {
                    case CredentialsType::INVALID:
                        return false;
                    case CredentialsType::PASSWORD:
                        {
                            const auto& storedPw = user.credentials.password;
                            const auto& inputPw = credentials.password;
                            if(crypto_pwhash_str_verify(storedPw.c_str(), inputPw.c_str(), inputPw.size()) != 0)
                            {
                                return false;
                            }
                            return true;
                        }
                    case CredentialsType::PASSKEY:
                        {
                        }
                        break;
                }
            }
        }
        return false;
    }

    bool UserStorage::containsUser(const UserName& name)
    {
        auto list = users.get();
        for(const auto& user : list)
        {
            if(user.name == name)
            {
                return true;
            }
        }
        return false;
    }

} // namespace tpunkt