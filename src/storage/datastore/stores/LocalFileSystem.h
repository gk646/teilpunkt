#ifndef TPUNKT_LOCAL_FILESYSTEM_DATASTORE_H
#define TPUNKT_LOCAL_FILESYSTEM_DATASTORE_H

#include <vector>
#include "storage/datastore/DataStore.h"

namespace tpunkt{

struct FileSystemDataStore final : DataStore
{
    explicit FileSystemDataStore(EndpointID endpoint);
    ~FileSystemDataStore() override;

    bool createFile(uint32_t fileID, ResultCallback callback) override;
    bool deleteFile(uint32_t fileID, ResultCallback callback) override;
    bool readFile(uint32_t fileID, ReadCallback callback) override;
    bool initWrite(uint32_t fileID, DatastoreHandle& handle) override;
    bool writeFile(DatastoreHandle handle, const unsigned char* data, size_t size, size_t idx,
                   ResultCallback callback) override;
    bool commitWrite(DatastoreHandle handle) override;

private:
    struct WriteCache final
    {
        DatastoreHandle handle;
        int fd; // File Descriptor
        size_t index;
        size_t chunkSize;
    };
    std::vector<WriteCache> writeCache;
    int dirfd; // Directory file descriptor
};

}
#endif //TPUNKT_LOCAL_FILESYSTEM_DATASTORE_H