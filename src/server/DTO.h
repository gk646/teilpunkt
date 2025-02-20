// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_DTO_H
#define TPUNKT_DTO_H

#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{

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

struct DTOFileInfo final
{
};

struct DTOFileDownload final
{
    FileID fileID;
    size_t begin;
    size_t end;
};

} // namespace tpunkt

#endif // TPUNKT_DTO_H