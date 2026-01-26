// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_FWD_H
#define TPUNKT_FWD_H

#include <sodium/utils.h>
#include <sodium/crypto_auth_hmacsha512.h>
#include <sodium/crypto_pwhash.h>
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
struct WriteFileTransaction;
struct FileCreationInfo;
struct VirtualFilesystem;
struct VirtualFile;
struct FileInfo;
struct StorageEndpoint;
struct FileStats;

namespace DTO
{

struct RequestUserLoginPassword;
struct RequestUserSignupPassword;
struct RequestUserSignupPasskey;
struct ResponseDirectoryInfo;
struct ResponseDirectoryEntry;
struct SessionInfo;
struct FileDownload;

} // namespace DTO

struct SessionMetaData;

//===== Fixed String =====//

template <size_t len>
struct FixedString;

// Needs to hold that many bytes encoded as base64
using SessionToken =
    FixedString<sodium_base64_ENCODED_LEN(TPUNKT_CRYPTO_SESSION_ID_LEN, sodium_base64_VARIANT_ORIGINAL_NO_PADDING)>;
using UserName = FixedString<TPUNKT_STORAGE_USER_LEN>;
using UserPassword = FixedString<crypto_pwhash_STRBYTES>;
using UserPasskey = FixedString<TPUNKT_AUTH_PASSKEY_LEN>;
using UserAgentString = FixedString<50>;
using HashedIP = FixedString<16>;
using ConfigString = FixedString<TPUNKT_STORAGE_FILE_LEN>;
using FileName = FixedString<TPUNKT_STORAGE_FILE_LEN>; // Max length for any single file name
using CipherKey = FixedString<TPUNKT_CRYPTO_KEY_LEN>;
using InstanceSecret = FixedString<TPUNKT_INSTANCE_SECRET_MAX_LEN>;
using TaskName = FixedString<25>;
// Encoded in base32 so need around 1.6 times more space
using TOTPKey = FixedString<crypto_auth_hmacsha512_KEYBYTES * 2>;
using TOTPCode = FixedString<16>;
using TOTPInfo = FixedString<256>;

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


} // namespace tpunkt

#endif // TPUNKT_FWD_H
