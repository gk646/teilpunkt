#ifndef TPUNKT_LOCAL_FILESYSTEM_DATASTORE_H
#define TPUNKT_LOCAL_FILESYSTEM_DATASTORE_H

#include "storage/datastore/DataStore.h"

namespace tpunkt
{
    struct FileSystemDataStore final : DataStore
    {
        explicit FileSystemDataStore(EndpointID endpoint);
        ~FileSystemDataStore() override;

        bool createFile(uint32_t fileID, ResultCb callback) override;

        bool deleteFile(uint32_t fileID, ResultCb callback) override;

        //===== Read =====//

        bool initRead(uint32_t fileID, size_t begin, size_t end, ReadHandle& handle) override;

        bool readFile(ReadHandle& handle, size_t chunkSize, ReadCb callback) override;

        bool closeRead(ReadHandle& handle, ResultCb callback) override;

        //===== Write =====//

        bool initWrite(uint32_t fileID, size_t begin, size_t end, WriteHandle& handle) override;

        bool writeFile(WriteHandle& handle, const unsigned char* data, size_t size, ResultCb callback) override;

        bool closeWrite(WriteHandle& handle, bool revert, ResultCb callback) override;

      private:
        int dirfd; // Directory file descriptor
    };

} // namespace tpunkt
#endif // TPUNKT_LOCAL_FILESYSTEM_DATASTORE_H