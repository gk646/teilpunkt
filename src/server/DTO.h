// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_DTO_H
#define TPUNKT_DTO_H

#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{

struct DirectoryEntryDTO
{
    FileName name;
    FileID file{};
    uint64_t lastEdit{};
    UserName creator;
    UserName owner;
};

struct DirectoryInfoDTO final
{
    FileName name;
    FileID dir{};
    uint32_t files; // Count of files visible to user
    uint32_t dirs;  // Count of directories visible to user
};


struct DTOFileDownload final
{
    FileID fileID;
    size_t begin;
    size_t end;
};

} // namespace tpunkt

#endif // TPUNKT_DTO_H