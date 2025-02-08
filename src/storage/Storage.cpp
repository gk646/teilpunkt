#include "storage/Storage.h"

namespace tpunkt
{

    FileID Storage::getNextFileID(const bool isDirectory, const EndpointID endPoint)
    {
        SpinlockGuard lock{storageLock};
        return FileID{.fileID = fileID++, .endpoint = endPoint, .isDirectory = isDirectory};
    }
} // namespace tpunkt