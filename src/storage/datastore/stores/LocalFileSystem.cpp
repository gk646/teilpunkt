#include <charconv>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/datastore/stores/LocalFileSystem.h"
#include "util/Logging.h"
#include "util/Macros.h"

constexpr int MAX_DIGITS = 12;

#define RET_AND_CB_FALSE()                                                                                             \
    callback(false);                                                                                                   \
    return false

namespace tpunkt
{
    FileSystemDataStore::FileSystemDataStore(const EndpointID endpoint) : DataStore(endpoint)
    {
        dirfd = open(dir.c_str(), O_RDONLY | O_DIRECTORY);
        if(dirfd == -1)
        {
            LOG_ERROR("open base directory failed");
        }
    }

    FileSystemDataStore::~FileSystemDataStore()
    {
        if(dirfd != -1)
        {
            close(dirfd);
        }
    }

    bool FileSystemDataStore::createFile(const uint32_t fileID, ResultCallback callback)
    {
        char buf[ MAX_DIGITS ];

        const auto result = std::to_chars(buf, buf + MAX_DIGITS, fileID);
        if(result.ec != std::errc()) [[unlikely]]
        {
            RET_AND_CB_FALSE();
        }
        *result.ptr = '\0';

        const int file = openat(dirfd, buf, O_CREAT | O_WRONLY, TPUNKT_INSTANCE_FILE_MODE);
        if(file == -1) [[unlikely]]
        {
            RET_AND_CB_FALSE();
        }

        if(close(file) == -1) [[unlikely]]
        {
            RET_AND_CB_FALSE();
        }

        callback(true);
        return true;
    }

    bool FileSystemDataStore::deleteFile(const uint32_t fileID, ResultCallback callback)
    {
        char buf[ MAX_DIGITS ];

        const auto result = std::to_chars(buf, buf + MAX_DIGITS, fileID);
        if(result.ec != std::errc()) [[unlikely]]
        {
            RET_AND_CB_FALSE();
        }
        *result.ptr = '\0';

        const int ret = unlinkat(dirfd, buf, 0);
        if(ret == -1) [[unlikely]]
        {
            RET_AND_CB_FALSE();
        }

        callback(true);
        return true;
    }

    bool FileSystemDataStore::readFile(uint32_t fileID, ReadCallback callback)
    {

    }

    bool FileSystemDataStore::initWrite(uint32_t fileID, DatastoreHandle& handle)
    {
    }

    bool FileSystemDataStore::writeFile(DatastoreHandle handle, const unsigned char* data, size_t size, size_t idx,
                                        ResultCallback callback)
    {
    }

    bool FileSystemDataStore::commitWrite(DatastoreHandle handle)
    {
    }

} // namespace tpunkt