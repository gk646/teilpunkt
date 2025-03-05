// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_DTO_H
#define TPUNKT_DTO_H

#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{

//===== Files =====//

struct DTOFileInfo final
{
    FileName name;
    uint64_t unixLastEdit = 0;
};

struct DTOFileDownload final
{
    FileID fileID;
    size_t begin;
    size_t end;
};

struct DTOFileStats final
{
    uint64_t unixLastEdit;
    uint64_t unixLastAccess;
    uint64_t unixCreation;

    uint64_t sizeBytes;
};

//===== Directories =====//

struct DTODirectoryEntry
{
    FileName name;
    FileID file{};
    uint64_t lastEdit{};
    UserName creator;
    UserName owner;
};

struct DTODirectoryInfo final
{
    FileName name;
    FileID dir{};
    uint32_t files = 0; // Count of files visible to user
    uint32_t dirs = 0;  // Count of directories visible to user
};


} // namespace tpunkt

#endif // TPUNKT_DTO_H