// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

#include <cstddef>
#include <cstdint>
#include "config.h"

namespace uWS
{
struct HttpRequest;
template <bool T>
struct HttpResponse;
struct Loop;
} // namespace uWS

namespace tpunkt
{

//===== Types =====//

struct User;
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
struct DataStore;
struct ReadTransaction;
struct StorageTransaction;
struct CreateTransaction;
struct DTOFileInfo;
struct DTOFileDownload;
struct FileCreationInfo;

template <typename T>
struct BlockStorage;

//===== Fixed String =====//

template <size_t len>
struct FixedString;
using SessionID = FixedString<TPUNKT_CRYPTO_SESSION_ID_LEN>;
using UserName = FixedString<TPUNKT_STORAGE_USER_LEN>;
using UserAgentString = FixedString<50>;
using HashedIP = FixedString<16>;
using ConfigString = FixedString<TPUNKT_STORAGE_FILE_LEN>;
using FileName = FixedString<TPUNKT_STORAGE_FILE_LEN>; // Max length for any single file name
using CipherKey = FixedString<TPUNKT_CRYPTO_KEY_LEN>;

//===== Identifiers =====//

enum TaskID : uint64_t
{
    INVALID = 0
};

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
    uint32_t file;
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