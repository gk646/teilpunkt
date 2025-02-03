#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>

#include "datastructures/FixedString.h"
#include "storage/VirtualFilesystem.h"
#include "storage/StorageTransaction.h"
#include "storage/DataStore.h"

namespace tpunkt
{
    enum class StorageStatus : uint8_t
    {
        INVALID,
        OK,
        ERR_NO_SUCH_ENDPOINT,
        ERR_UNSUPPORTED_OPERATION,
    };

    enum class StorageEndpointType : uint8_t
    {
        LOCAL_FILE_SYSTEM,
        REMOTE_FILE_SYSTEM,
    };

    struct StorageEndpoint
    {
        virtual ~StorageEndpoint() = default;

        //===== File Manipulation =====//

        //StorageStatus addFile(const FileDescriptor& descriptor, uint64_t size);
        StorageStatus removeFile();
        StorageStatus changeFile();
        StorageStatus renameFile();

        StorageStatus addDirectory();
        StorageStatus removeDirectory();

        StorageStatus changeDirectory();
        StorageStatus renameDirectory();

        StorageStatus clear();

        //===== Storage Info =====//

        virtual StorageStatus canBeAdded();

      private:
        DataStore* dataStore;
        VirtualFilesystem virtualFilesystem;
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        StorageEndpointType type{};
    };

} // namespace tpunkt

#endif // TPUNKT_STORAGE_ENDPOINT_H