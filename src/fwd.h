#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

#include <cstdint>
#include <cstddef>
#include "config.h"

namespace tpunkt
{
    struct User;
    struct File;
    struct Credentials;
    struct AuthToken;
    struct Authenticator;
    struct UserStorage;
    struct SessionStorage;
    template <typename T>
    struct SecureBox;
    template <typename T>
    struct SecureList;
    struct InstanceConfig;

    template <typename T>
    struct Collector;
    // Fixed string typedefs
    template <size_t len>
    struct FixedString;
    using SessionID = FixedString<TPUNKT_CRYPTO_SESSION_ID_LEN>;
    using UserName = FixedString<TPUNKT_STORAGE_USER_LEN>;
    using UserAgentString = FixedString<50>;
    using HashedIP = FixedString<16>;
    using ConfigString = FixedString<50>;
    using FileName = FixedString<63>;         // Max length for any single file name
    using FileSystemPath = FixedString<1023>; // Max length of any file path including the file name

    enum class UserID : uint32_t
    {
        INVALID = 0
    };

    enum class FileID : uint32_t
    {
        INVALID = 0
    };

    enum class GroupID : uint16_t
    {
        INVALID = 0
    };

    enum class DirectoryID : uint32_t
    {
        INVALID = 0
    };

} // namespace tpunkt

#endif // TPUNKT_FWD_H