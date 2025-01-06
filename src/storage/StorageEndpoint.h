#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>
#include "datastructures/FixedString.h"
#include "storage/VirtualFilesystem.h"

namespace tpunkt
{
    enum class StorageEndpointType : uint8_t
    {
        LOCAL_FILE_SYSTEM,
        REMOTE_FILE_SYSTEM,
    };

    struct StorageEndpoint
    {
        VirtualFilesystem virtualFilesystem;
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        StorageEndpointType type;

        StorageStatus addFile();
        StorageStatus removeFile();
        StorageStatus changeFile();
        StorageStatus renameFile();

        StorageStatus addDirectory();
        StorageStatus removeDirectory();
        StorageStatus changeDirectory();
        StorageStatus renameDirectory();

        StorageStatus clear();
    };

    struct LocalFileSystemEndpoint final : StorageEndpoint
    {
    };

} // namespace tpunkt
#endif //TPUNKT_STORAGE_ENDPOINT_H