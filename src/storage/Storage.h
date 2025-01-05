#ifndef TPUNK_STORAGE_H
#define TPUNK_STORAGE_H

#include <cstdint>
#include "storage/File.h"

namespace tpunkt
{
    enum class StorageStatus : uint8_t
    {
        INVALID = 0,
        OK = 1,
        ERR_NOT_INITIALIZED,
        ERR_NOT_ENOUGH_SPACE,
        ERR_ACTION_DENIED,
    };


    struct Storage final
    {

    };

} // namespace tpunkt

#endif //TPUNK_STORAGE_H