// SPDX-License-Identifier: GPL-3.0-only

#include <sodium/randombytes.h>
#include "auth/UserStorage.h"

namespace tpunkt
{
constexpr size_t PW_HASH_OPS_LVL = crypto_pwhash_OPSLIMIT_MODERATE;
constexpr size_t PW_HASH_MEM_LVL = crypto_pwhash_MEMLIMIT_INTERACTIVE;

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
                if(crypto_pwhash_str(storePw, newPw, newPwLen, PW_HASH_OPS_LVL, PW_HASH_MEM_LVL) != 0)
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

    // Assign id
    auto idReader = currUserID.get();
    newUser.userID = static_cast<UserID>(idReader.get());
    idReader.get()++; // Increment for next user

    return true;
}

bool UserStorage::remove(const UserID user)
{
    for(auto& userBox : users)
    {
        bool found = false;
        {
            auto boxReader = userBox.get();
            found = boxReader.get().userID == user;
        }
        if(found)
        {
            userBox = std::move(users.back());
            users.pop_back();
            return true;
        }
    }
    return false;
}

bool UserStorage::login(const UserName& name, const Credentials& credentials, UserID& user) const
{
    for(const auto& box : users)
    {
        const auto boxReader = box.get();
        const auto& userData = boxReader.get();
        if(userData.name == name)
        {
            switch(credentials.type)
            {
                case CredentialsType::INVALID:
                    return false;
                case CredentialsType::PASSWORD:
                    {
                        const auto& storedPw = userData.credentials.password;
                        const auto& inputPw = credentials.password;
                        if(crypto_pwhash_str_verify(storedPw.c_str(), inputPw.c_str(), inputPw.size()) != 0)
                        {
                            return false;       // Failed comparison
                        }
                        user = userData.userID; // Save the userID
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

bool UserStorage::changeCredentials(const UserID user, const UserName& newName, const Credentials& newCredentials)
{
    for(auto& box : users)
    {
        auto boxReader = box.get();
        auto& userData = boxReader.get();
        if(userData.userID == user)
        {
            if(userData.credentials == newCredentials)                // No need to assign new credentials
            {
                userData.name = newName;                              // Always assign new name
                return true;
            }

            switch(newCredentials.type)
            {
                case CredentialsType::INVALID:
                    return false;
                case CredentialsType::PASSWORD:
                    {
                        Credentials tempCredentials = newCredentials; // Store into local copy
                        char* storePw = tempCredentials.password.data();
                        const char* newPw = newCredentials.password.c_str();
                        const auto newPwLen = newCredentials.password.size();
                        if(crypto_pwhash_str(storePw, newPw, newPwLen, PW_HASH_OPS_LVL, PW_HASH_MEM_LVL) != 0)
                        {
                            return false;                             // Out of memory - user password didn't change
                        }
                        // Successful operation - assigns both
                        userData.credentials = tempCredentials;
                        userData.name = newName;
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

UserStorage::UserStorage()
{
    auto idReader = currUserID.get();
    do
    {
        idReader.get() = randombytes_random() / 2U; // At least 2.1 billion entries - non zero
    } while(idReader.get() == 0U);
    // IO
}

UserStorage::~UserStorage()
{
    // IO
}

} // namespace tpunkt
