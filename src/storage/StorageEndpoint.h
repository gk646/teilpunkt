#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>

#include "datastructures/FixedString.h"
#include "storage/vfs/VirtualFilesystem.h"
#include "storage/datastore/DataStore.h"
#include "storage/datastore/StorageTransaction.h"

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

         StorageStatus addFile(UserID user, FileID dir, const FileCreationInfo& info,
        StorageTransaction& transaction);
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
        VirtualFilesystem virtualFilesystem;
        DataStore* dataStore = nullptr;
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        StorageEndpointType type{};
    };

} // namespace tpunkt

#endif // TPUNKT_STORAGE_ENDPOINT_H