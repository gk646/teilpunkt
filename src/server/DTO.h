// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_DTO_H
#define TPUNKT_DTO_H

#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt::DTO
{

//===== Authentication =====//

struct UserSignupPW final
{
    UserName name;
    UserPassword password;
};

struct UserSignupPK final
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
    struct
    {
        int alg = -8;
        FixedString<TPUNKT_STORAGE_FILE_LEN> type{"public-key"};
    } pubKeyCredParams;
};


struct UserLoginPW final
{
    UserName name;
    UserPassword password;
};


//===== Filesystem =====//


struct DirectoryRequest final
{
    // The user is already gained through auth with the cookie
    FileID directory;
};

struct CreateRequest final
{
    FileName name;
    FileID directory;
};

struct DirectoryInfo
{
    static DirectoryInfo FromDir(const VirtualDirectory& dir);

    FileName name;
    FileID fid;
};

struct DirectoryEntry final
{
    static DirectoryEntry FromFile(const VirtualFile& file);
    static DirectoryEntry FromDir(const VirtualDirectory& dir);

    FileName name;
    FileID fid;
    bool isFile;

    UserName creator;
    UserName owner;

    uint64_t unixLastEdit = 0;
    uint64_t unixLastAccess = 0;
    uint64_t unixCreation = 0;

    uint64_t sizeBytes = 0;
};

//===== User =====//

struct SessionInfo final
{
    UserAgentString userAgent;
    uint64_t creationUnix = 0;
    uint64_t expirationUnix = 0;
};

//===== Directories =====//


//===== Requests =====//


//===== Task Management =====//

struct TaskInfo final
{
    TaskName name;                 // Name of the task
    uint64_t addedUnixNanos = 0;
    uint64_t startUnixNanos = 0;
    uint64_t doneUnixNanos = 0;
    uint32_t threadID = 0;         // 0 means task is only scheduled not taken
    uint32_t taskID = 0;
};

struct ThreadInfo final
{
    uint64_t startUnixNanos = 0;   // Thread start
    uint32_t threadID = 0;
    uint32_t currentTaskID = 0;    // 0 means no task
    bool isWorking = false;
};

struct TaskManagerInfo final
{
    uint32_t threadsCurrent = 0;   // Current threads in use
    uint32_t threadsMax = 0;       // Global maximum of used threads
    uint32_t threadsAvailable = 0; // Max available
};

} // namespace tpunkt::DTO


#endif // TPUNKT_DTO_H
