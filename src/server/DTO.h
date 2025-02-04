#ifndef TPUNKT_DTO_H
#define TPUNKT_DTO_H

#include "fwd.h"
#include "datastructures/FixedString.h"


namespace tpunkt{

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
    };

    }

#endif //TPUNKT_DTO_H