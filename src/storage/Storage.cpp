#include "storage/Storage.h"

namespace tpunkt
{

    FileID Storage::getNextFileID(const bool isDirectory, const uint8_t endPoint)
    {
        SpinlockGuard lock{storageLock};
        return FileID{.fileID=fileID++, .isDirectory=isDirectory, .endpoint=EndpointID{ endPoint}};
    }
} // namespace tpunkt