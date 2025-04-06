// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_DTO_H
#define TPUNKT_DTO_H

#include "auth/Credentials.h"
#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{

//===== Signup =====//

struct DTOUserSignupPW final
{
    UserName name;
    UserPassword password;
};

struct DTOUserSignupPK final
{
    FixedString<TPUNKT_CRYPTO_KEY_LEN> challenge;
    struct
    {
        FixedString<TPUNKT_STORAGE_FILE_LEN> id;
        FixedString<TPUNKT_STORAGE_FILE_LEN> name;
    } rp; // relaying party
    struct
    {
        UserName displayName;
        UserName id;
        UserName name;

    } user;
    struct PubKeyCredParams final
    {
        int alg;
        FixedString<TPUNKT_STORAGE_FILE_LEN> type{"public-key"};
    };
    PubKeyCredParams pubKeyCredParams[ 3 ]{{-8}, {-7}, {-257}};
};

//===== Login =====//

struct DTOUserLoginPW final
{
    UserName name;
    UserPassword password;
};

//===== User =====//

struct DTOSessionInfo final
{
    UserAgentString userAgent;
    uint64_t creationUnix = 0;
    uint64_t expirationUnix = 0;
};

//===== Directories =====//

struct DTODirectoryInfo
{
    FileName name;
    FileID fid;
};

struct DTODirectoryEntry final
{
    FileName name;
    FileID fid;

    UserName creator;
    UserName owner;

    uint64_t unixLastEdit = 0;
    uint64_t unixLastAccess = 0;
    uint64_t unixCreation = 0;

    uint64_t sizeBytes = 0;
};


//===== Requests =====//

struct DTODirectoryRequest final
{
    // The user is already gained through auth with the cookie
    FileID directory;
};


} // namespace tpunkt

#endif // TPUNKT_DTO_H