#include <charconv>
#include <fcntl.h>
#include <unistd.h>
#include "datastructures/FixedString.h"
#include "storage/datastore/stores/LocalFileSystem.h"
#include "util/Logging.h"

static constexpr int MAX_DIGITS = 14;

#define RET_AND_CB_FALSE()                                                                                             \
    callback(false);                                                                                                   \
    return false

#define RET_AND_READ_CB_FALSE()                                                                                        \
    callback(nullptr, 0, false, true);                                                                                 \
    return false


namespace tpunkt
{
    FileSystemDataStore::FileSystemDataStore(const EndpointID endpoint) : DataStore(endpoint)
    {
        dirfd = open(dir.c_str(), O_RDONLY | O_DIRECTORY);
        if(dirfd == -1)
        {
            LOG_CRITICAL("Opening Datastore base directory failed");
        }
    }

    FileSystemDataStore::~FileSystemDataStore()
    {
        if(dirfd != -1)
        {
            close(dirfd);
        }
    }

    bool FileSystemDataStore::createFile(const uint32_t fileID, ResultCb callback)
    {
        char buf[ MAX_DIGITS ];

        const auto result = std::to_chars(buf, buf + MAX_DIGITS, fileID);
        if(result.ec != std::errc()) [[unlikely]]
        {
            RET_AND_CB_FALSE();
        }
        *result.ptr = '\0';

        const int file = openat(dirfd, buf, O_CREAT | O_EXCL | O_WRONLY, TPUNKT_INSTANCE_FILE_MODE);
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

    bool FileSystemDataStore::deleteFile(const uint32_t fileID, ResultCb callback)
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

    bool FileSystemDataStore::initRead(const uint32_t fileID, const size_t begin, const size_t end, ReadHandle& handle)
    {
        char buf[ MAX_DIGITS ];
        {
            const auto result = std::to_chars(buf, buf + MAX_DIGITS, fileID);
            if(result.ec != std::errc()) [[unlikely]]
            {
                return false;
            }
            *result.ptr = '\0';
        }

        const int file = openat(dirfd, buf, O_RDONLY);
        if(file == -1) [[unlikely]]
        {
            return false;
        }

        handle.buffer = UINT8_MAX;
        for(uint8_t i = 0; i < TPUNKT_STORAGE_DATASTORE_MAX_READERS; ++i)
        {
            if(buffers[ i ].lock())
            {
                handle.buffer = i;
            }
        }

        if(handle.buffer == UINT8_MAX)
        {
            return false;
        }

        handle.position = begin;
        handle.end = end;
        handle.fd = file;
        handle.fileID = fileID;

        return true;
    }

    bool FileSystemDataStore::readFile(ReadHandle& handle, const size_t chunkSize, ReadCb callback)
    {
        if(!handle.isValid())
        {
            LOG_ERROR("Passed invalid handle");
            RET_AND_READ_CB_FALSE();
        }

        if(handle.isDone())
        {
            LOG_WARNING("Passed finished handle");
            RET_AND_READ_CB_FALSE();
        }

        auto& buffer = buffers[ handle.buffer ].getBuf();
        buffer.ensure(chunkSize);

        const auto request = std::min(chunkSize, handle.end - handle.position);
        const auto result = pread64(handle.fd, buffer.data(), request, static_cast<int64_t>(handle.position));

        if(result == -1)
        {
            RET_AND_READ_CB_FALSE();
        }
        handle.position += result;

        const bool isLast = (result == 0) || (handle.position >= handle.end);
        callback(buffer.data(), result, true, isLast);

        return true;
    }


    bool FileSystemDataStore::closeRead(ReadHandle& handle, ResultCb callback)
    {
        if(!handle.isValid())
        {
            return false; // We dont touch as its invalid
        }

        if(!handle.isDone())
        {
            LOG_WARNING("Closing Read prematurely");
        }

        if(handle.fd != -1)
        {
            const int result = close(handle.fd);
            if(result == -1)
            {
                LOG_ERROR("Failed to close file");
            }
        }
        buffers[ handle.buffer ].unlock();
    }


    bool FileSystemDataStore::initWrite(const uint32_t fileID, const size_t begin, const size_t end,
                                        WriteHandle& handle)
    {
        char buf[ MAX_DIGITS ];
        {
            const auto result = std::to_chars(buf, buf + MAX_DIGITS, fileID);
            if(result.ec != std::errc()) [[unlikely]]
            {
                return false;
            }
            *result.ptr = '\0';
        }

        const int file = openat(dirfd, buf, O_WRONLY);
        if(file == -1) [[unlikely]]
        {
            return false;
        }

        {
            const auto result = std::to_chars(buf, buf + MAX_DIGITS, fileID);
            if(result.ec != std::errc()) [[unlikely]]
            {
                return false;
            }
            result.ptr[ 0 ] = 'W'; // Write file
            result.ptr[ 1 ] = '\0';
        }

        const int tempFile = openat(dirfd, buf, O_CREAT | O_TRUNC);
        if(tempFile == -1) [[unlikely]]
        {
            return false;
        }

        handle.buffer = 0;         // Not needed
        handle.position = begin;
        handle.end = end;
        handle.targetFile = file;
        handle.tempFile = tempFile;
        handle.fileID = fileID;

        return true;
    }

    bool FileSystemDataStore::writeFile(WriteHandle& handle, const unsigned char* data, const size_t size, ResultCb callback)
    {
        if(!handle.isValid())
        {
            return false; // We dont touch as its invalid
        }

        if(!handle.isDone())
        {
            LOG_WARNING("Closing Read prematurely");
        }

        const auto result = pwrite64(handle.tempFile, data, size, handle.position);
        handle.position += result;

        if()
    }

    bool FileSystemDataStore::closeWrite(WriteHandle& handle, const bool revert, ResultCb callback)
    {
        if(revert)
        {
        }
        else
        {
        }
    }


} // namespace tpunkt