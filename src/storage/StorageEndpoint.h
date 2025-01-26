#ifndef TPUNKT_STORAGE_ENDPOINT_H
#define TPUNKT_STORAGE_ENDPOINT_H

#include <cstdint>

#include "datastructures/FixedString.h"
#include "storage/VirtualFilesystem.h"
#include "storage/Transaction.h"

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

        virtual StorageStatus addFile(const FileDescriptor& descriptor, uint64_t size) = 0;
        virtual StorageStatus removeFile();
        virtual StorageStatus changeFile();
        virtual StorageStatus renameFile();

        virtual StorageStatus addDirectory();
        virtual StorageStatus removeDirectory();
        virtual StorageStatus changeDirectory();
        virtual StorageStatus renameDirectory();

        StorageStatus clear();

        //===== Storage Info =====//

        virtual StorageStatus canBeAdded();

      private:
        VirtualFilesystem virtualFilesystem;
        FixedString<TPUNKT_STORAGE_NAME_LEN> name;
        StorageEndpointType type{};
    };

    struct LocalFileSystemEndpoint final : StorageEndpoint
    {
    };

} // namespace tpunkt
#endif // TPUNKT_STORAGE_ENDPOINT_H