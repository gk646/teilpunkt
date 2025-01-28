#ifndef TPUNKT_STORAGEDTOS_H
#define TPUNKT_STORAGEDTOS_H

#include "fwd.h"
#include "datastructures/FixedString.h"

namespace tpunkt
{
    struct DirectoryEntryDTO final
    {
        FileName name;
        FileID file{};
        uint64_t lastEdit;
        UserName creator;
        UserName owner;
    };

    struct DirectoryInfoDTO final
    {
        FileName name;
        FileID dir{};
    };

} // namespace tpunkt

#endif // TPUNKT_STORAGEDTOS_H