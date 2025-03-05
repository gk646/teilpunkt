// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/datastore/LocalFileSystem.h"
#include "util/Logging.h"
#include "util/Strings.h"

static constexpr int MAX_DIGITS = 14;

#define RET_AND_CB_FALSE()                                                                                             \
    callback(false);                                                                                                   \
    return false

#define RET_AND_READ_CB_FALSE()                                                                                        \
    callback(nullptr, 0, false, true);                                                                                 \
    return false

namespace tpunkt
{

LocalFileSystemDatastore::LocalFileSystemDatastore(const EndpointID endpoint, bool& success)
    : DataStore(endpoint, success), dirfd(open(dir.c_str(), O_RDONLY | O_DIRECTORY))
{
    if(dirfd == -1)
    {
        LOG_ERROR("Opening Datastore base directory failed");
        success = false;
    }
}

LocalFileSystemDatastore::~LocalFileSystemDatastore()
{
    if(dirfd != -1)
    {
        close(dirfd);
        dirfd = -1;
    }
}

bool LocalFileSystemDatastore::createFile(const uint32_t fileID, ResultCb callback)
{
    char buf[ MAX_DIGITS ];
    if(!NumberToString(buf, MAX_DIGITS, fileID))
    {
        RET_AND_CB_FALSE();
    }

    const int file = openat(dirfd, buf, O_CREAT | O_EXCL | O_WRONLY, TPUNKT_INSTANCE_FILE_MODE);
    if(file == -1) [[unlikely]]
    {
        LOG_ERROR("Creating file failed: %s", strerror(errno));
        RET_AND_CB_FALSE();
    }

    if(close(file) == -1) [[unlikely]]
    {
        RET_AND_CB_FALSE();
    }

    callback(true);
    return true;
}

bool LocalFileSystemDatastore::deleteFile(const uint32_t fileID, ResultCb callback)
{
    char buf[ MAX_DIGITS ];
    if(!NumberToString(buf, MAX_DIGITS, fileID))
    {
        RET_AND_CB_FALSE();
    }

    const int ret = unlinkat(dirfd, buf, 0);
    if(ret == -1) [[unlikely]]
    {
        LOG_ERROR("Deleting file failed: %s", strerror(errno));
        RET_AND_CB_FALSE();
    }

    callback(true);
    return true;
}

bool LocalFileSystemDatastore::initRead(const uint32_t fileID, const size_t begin, const size_t end, ReadHandle& handle)
{
    if(end != 0 && begin >= end)
    {
        LOG_WARNING("Invalid read request");
        return false;
    }

    char buf[ MAX_DIGITS ];
    if(!NumberToString(buf, MAX_DIGITS, fileID))
    {
        return false;
    }

    const int file = openat(dirfd, buf, O_RDONLY | O_EXCL);
    if(file == -1) [[unlikely]]
    {
        LOG_ERROR("Opening file failed: %s", strerror(errno));
        return false;
    }

    handle.buffer = UINT8_MAX;
    for(uint8_t i = 0; i < TPUNKT_STORAGE_DATASTORE_MAX_READERS; ++i)
    {
        if(buffers[ i ].lock())
        {
            handle.buffer = i;
            break;
        }
    }

    if(handle.buffer == UINT8_MAX)
    {
        return false;
    }

    handle.end = end;
    handle.position = begin;
    handle.fd = file;
    handle.fileID = fileID;

    if(end == 0)
    {
        struct stat fileStat{};
        if(fstat(file, &fileStat) == -1)
        {
            LOG_ERROR("Getting file size failed: %s", strerror(errno));
            return false;
        }
        handle.end = fileStat.st_size;
    }

    return true;
}

bool LocalFileSystemDatastore::readFile(ReadHandle& handle, const size_t chunkSize, ReadCb callback)
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
    const auto read = pread64(handle.fd, buffer.data(), request, static_cast<int64_t>(handle.position));
    if(read == -1)
    {
        LOG_ERROR("Reading file failed: %s", strerror(errno));
        RET_AND_READ_CB_FALSE();
    }
    handle.position += read;

    const bool isLast = (read == 0) || (handle.position >= handle.end);
    if(isLast)
    {
        handle.position = SIZE_MAX;
    }

    callback(buffer.data(), read, true, isLast);
    return true;
}

bool LocalFileSystemDatastore::closeRead(ReadHandle& handle, ResultCb callback)
{
    if(!handle.isValid())
    {
        RET_AND_CB_FALSE(); // We dont touch as its invalid
    }

    if(!handle.isDone())
    {
        LOG_WARNING("Closing Read prematurely");
    }

    bool success = true;
    if(handle.fd != -1)
    {
        if(close(handle.fd) == -1)
        {
            LOG_ERROR("Failed to close file: %s", strerror(errno));
            success = false;
        }
        handle.fd = -1;
    }
    buffers[ handle.buffer ].unlock();

    callback(success);
    return success;
}

bool LocalFileSystemDatastore::initWrite(const uint32_t fileID, WriteHandle& handle)
{
    char buf[ MAX_DIGITS ];
    if(!NumberToString(buf, MAX_DIGITS, fileID))
    {
        return false;
    }
    const int target = openat(dirfd, buf, O_WRONLY);
    if(target == -1) [[unlikely]]
    {
        return false;
    }

    if(!NumberToStringEx(buf, MAX_DIGITS, fileID, "T"))
    {
        return false;
    }
    const int tempFile = openat(dirfd, buf, O_CREAT | O_TRUNC | O_WRONLY, TPUNKT_INSTANCE_FILE_MODE);
    if(tempFile == -1) [[unlikely]]
    {
        LOG_ERROR("Opening target file failed: %s", strerror(errno));
        return false;
    }

    handle.buffer = 0; // Not needed for local file system
    handle.tempPosition = 0;
    handle.targetfd = target;
    handle.tempfd = tempFile;
    handle.fileID = fileID;

    return true;
}

bool LocalFileSystemDatastore::writeFile(WriteHandle& handle, const bool isLast, const unsigned char* data,
                                         const size_t size, ResultCb callback)
{
    if(!handle.isValid())
    {
        RET_AND_CB_FALSE(); // We dont touch as its invalid
    }

    if(handle.isDone())
    {
        LOG_WARNING("Passed finished handle");
        RET_AND_CB_FALSE();
    }

    const auto written = pwrite64(handle.tempfd, data, size, static_cast<int64_t>(handle.tempPosition));
    if(written == -1)
    {
        RET_AND_CB_FALSE();
    }

    handle.tempPosition += static_cast<size_t>(written);

    if(size != written) // Failed to write as many as requested
    {
        RET_AND_CB_FALSE();
    }

    if(isLast)
    {
        handle.done = true;
    }

    callback(true);
    return true;
}

bool LocalFileSystemDatastore::closeWrite(WriteHandle& handle, const bool revert, ResultCb callback)
{
    if(!handle.isValid())
    {
        RET_AND_CB_FALSE(); // We dont touch as its invalid
    }

    if(!handle.isDone())
    {
        LOG_WARNING("Closing unfinished Write");
    }

    bool success = true;
    char buf[ MAX_DIGITS ];

    if(revert)              // Revert the transaction - only need to delete temp file
    {
        if(!NumberToStringEx(buf, MAX_DIGITS, handle.fileID, "T"))
        {
            success = false;
        }
        else if(unlinkat(dirfd, buf, 0) == -1)
        {
            LOG_ERROR("Deleting temp file failed: %s", strerror(errno));
            success = false;
        }
    }
    else // Apply transaction - rename temp
    {
        if(!NumberToString(buf, MAX_DIGITS, handle.fileID))
        {
            success = false;
        }
        else if(unlinkat(dirfd, buf, 0) == -1)
        {
            LOG_ERROR("Deleting target file failed: %s", strerror(errno));
            success = false;
        }

        char tempBuf[ MAX_DIGITS ];
        if(!NumberToStringEx(tempBuf, MAX_DIGITS, handle.fileID, "T"))
        {
            success = false;
        }
        else if(renameat(dirfd, tempBuf, dirfd, buf) == -1)
        {
            LOG_ERROR("Renaming temp file to target file failed: %s", strerror(errno));
            success = false;
        }
    }

    if(handle.targetfd != -1)
    {
        if(close(handle.targetfd) == -1)
        {
            LOG_ERROR("Failed to close target file descriptor: %s", strerror(errno));
            success = false;
        }
        handle.targetfd = -1;
    }

    if(handle.tempfd != -1)
    {
        if(close(handle.tempfd) == -1)
        {
            LOG_ERROR("Failed to close temp file descriptor: %s", strerror(errno));
            success = false;
        }
        handle.tempfd = -1;
    }

    callback(success);
    return success;
}

} // namespace tpunkt