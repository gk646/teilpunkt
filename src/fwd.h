#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

#include <cstddef>
#include <cstdint>
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
    struct Storage;
    struct VirtualDirectory;

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
    using FileName = FixedString<31>;         // Max length for any single file name
    using FileSystemPath = FixedString<1023>; // Max length of any file path including the file name

    enum class UserID : uint32_t
    {
        INVALID = 0
    };

    enum class EndpointID : uint16_t
    {
        INVALID = 0
    };

    struct FileID final
    {
        uint32_t fileID;
        EndpointID endpoint;
        bool isDirectory;
        // 1 byte left
        bool operator==(const FileID&) const = default;
    };

    enum class GroupID : uint16_t
    {
        INVALID = 0
    };


} // namespace tpunkt

#endif // TPUNKT_FWD_H