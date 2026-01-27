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

LocalFileSystemDatastore::LocalFileSystemDatastore(const EndpointID endpoint)
    : DataStore(endpoint), dirfd(open(dir.c_str(), O_RDONLY | O_DIRECTORY))
{
    if(dirfd == -1)
    {
        LOG_ERROR("Opening Datastore base directory failed: %s", strerror(errno));
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
    FixedString<MAX_DIGITS> name{fileID};
    const int file = openat(dirfd, name.c_str(), O_CREAT | O_EXCL | O_WRONLY, TPUNKT_INSTANCE_FILE_MODE);
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
    FixedString<MAX_DIGITS> name{fileID};
    const int ret = unlinkat(dirfd, name.c_str(), 0);
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

    FixedString<MAX_DIGITS> name{fileID};
    const int file = openat(dirfd, name.c_str(), O_RDONLY | O_EXCL);
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
    FixedString<MAX_DIGITS> targetName{fileID};
    const int target = openat(dirfd, targetName.c_str(), O_WRONLY);
    if(target == -1) [[unlikely]]
    {
        LOG_ERROR("Opening target file failed: %s", strerror(errno));
        return false;
    }

    FixedString<MAX_DIGITS> tempName{fileID, "T"};
    const int tempFile = openat(dirfd, tempName.c_str(), O_CREAT | O_TRUNC | O_WRONLY, TPUNKT_INSTANCE_FILE_MODE);
    if(tempFile == -1) [[unlikely]]
    {
        LOG_ERROR("Opening temp file failed: %s", strerror(errno));
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
    callback(true);

    if(!handle.isValid())
    {
        RET_AND_CB_FALSE(); // We dont touch as its invalid
    }

    if(!handle.isDone())
    {
        LOG_WARNING("Closing unfinished Write");
    }

    bool success = true;
    if(revert)              // Revert the transaction - only need to delete temp file
    {
        FixedString<MAX_DIGITS> tempName{handle.fileID, "T"};
        if(unlinkat(dirfd, tempName.c_str(), 0) == -1)
        {
            LOG_ERROR("Deleting temp file failed: %s", strerror(errno));
            success = false;
        }
    }
    else                    // Apply transaction - rename temp
    {
        FixedString<MAX_DIGITS> name{handle.fileID};
        if(unlinkat(dirfd, name.c_str(), 0) == -1)
        {
            LOG_ERROR("Deleting target file failed: %s", strerror(errno));
            success = false;
        }

        FixedString<MAX_DIGITS> tempName{handle.fileID, "T"};
        if(renameat(dirfd, tempName.c_str(), dirfd, name.c_str()) == -1)
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

    return success;
}

} // namespace tpunkt
