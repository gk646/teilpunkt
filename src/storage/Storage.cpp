#include "storage/Storage.h"

namespace tpunkt
{

    uint32_t Storage::getNextFileID()
    {
        SpinlockGuard lock{storageLock};
        return fileID++;
    }
} // namespace tpunkt