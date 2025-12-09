// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

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
struct ReadFileTransaction;
struct StorageTransaction;
struct CreateFileTransaction;
struct DTOFileInfo;
struct DTOFileDownload;
struct FileCreationInfo;
struct VirtualFilesystem;
struct VirtualFile;
struct DTODirectoryInfo;
struct DTOFileStats;
struct DTOSessionInfo;
struct SessionMetaData;

//===== Fixed String =====//

template <size_t len>
struct FixedString;
using SessionToken = FixedString<TPUNKT_CRYPTO_SESSION_ID_LEN>;
using UserName = FixedString<TPUNKT_STORAGE_USER_LEN>;
using UserPassword = FixedString<TPUNKT_AUTH_PASSWORD_LEN>;
using UserPasskey = FixedString<TPUNKT_AUTH_PASSKEY_LEN>;
using UserAgentString = FixedString<50>;
using HashedIP = FixedString<16>;
using ConfigString = FixedString<TPUNKT_STORAGE_FILE_LEN>;
using FileName = FixedString<TPUNKT_STORAGE_FILE_LEN>; // Max length for any single file name
using CipherKey = FixedString<TPUNKT_CRYPTO_KEY_LEN>;
using InstanceSecret = FixedString<TPUNKT_INSTANCE_SECRET_MAX_LEN>;
using TaskName = FixedString<25>;
using TimedOneTimeKey = FixedString<32>;
using TimedOneTimeCode = FixedString<32>;

//===== Identifiers =====//

enum TaskID : uint32_t
{
    INVALID = 0
};

enum class UserID : uint32_t
{
    INVALID = 0,
    SERVER = 1,
};

enum class EndpointID : uint16_t
{
    INVALID = 0
};

enum class GroupID : uint16_t
{
    INVALID = 0
};

enum class FileID : uint32_t
{
    INVALID = 0
};

} // namespace tpunkt

#endif // TPUNKT_FWD_H
