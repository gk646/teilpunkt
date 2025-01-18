#include "auth/UserStorage.h"
#include "util/FileIO.h"

namespace tpunkt
{
    UserStorage::UserStorage()
    {
        // IO
    }

    UserStorage::~UserStorage()
    {
        // IO
    }

    bool UserStorage::add(const UserName& name, const Credentials& credentials)
    {
        Credentials newCredentials = credentials;
        switch(credentials.type)
        {
            case CredentialsType::INVALID:
                return false;
            case CredentialsType::PASSWORD:
                {
                    char* storePw = newCredentials.password.data();
                    const char* newPw = credentials.password.c_str();
                    const auto newPwLen = credentials.password.size();
                    constexpr auto calcDiff = crypto_pwhash_OPSLIMIT_MODERATE;
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

        // Add user to user list
        auto boxReader = users.emplace_back().get();
        auto& newUser = boxReader.get();
        newUser.name = name;
        newUser.credentials = newCredentials;

        return true;
    }

    bool UserStorage::remove(const UserName& name)
    {
        for(auto it = users.begin(); it != users.end(); ++it)
        {
            const auto boxReader = it->get();
            if(boxReader.get().name == name)
            {
                *it = std::move(users.back());
                users.pop_back();
                return true;
            }
        }
        return false;
    }

    bool UserStorage::login(const UserName& name, const Credentials& credentials, const SecureBox<User>*& loginUser)
    {
        for(const auto& box : users)
        {
            const auto boxReader = box.get();
            const auto& user = boxReader.get();
            if(user.name == name)
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
                                return false; // Failed comparison
                            }
                            loginUser = &box; // Save the box
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

    bool UserStorage::nameExists(const UserName& name) const
    {
        for(const auto& box : users)
        {
            const auto boxReader = box.get();
            const auto& user = boxReader.get();
            if(user.name == name)
            {
                return true;
            }
        }
        return false;
    }

} // namespace tpunkt