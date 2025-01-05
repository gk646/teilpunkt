#ifndef TPUNKT_STORAGEENDPOINT_H
#define TPUNKT_STORAGEENDPOINT_H

#include <cstdint>

namespace tpunkt
{

    enum class StorageEndpointType : uint8_t
    {
        LOCAL_FILE_SYSTEM,
    };

    struct StorageEndpoint
    {
        StorageEndpointType type;
    };

    struct LocalFileSystemEndpoint final : StorageEndpoint
    {
    };

} // namespace tpunkt
#endif //TPUNKT_STORAGEENDPOINT_H