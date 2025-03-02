// SPDX-License-Identifier: Apache License 2.0

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "crypto/Ciphers.h"
#include "instance/InstanceInfo.h"
#include "util/FileIO.h"
#include "util/Logging.h"

namespace tpunkt
{

File::File(const char* path)
{
    fd = openat(GetInstanceInfo().getRoot(), path, O_CREAT | O_RDWR, TPUNKT_INSTANCE_FILE_MODE);
    if(fd == -1) [[unlikely]]
    {
        LOG_ERROR("Creating file failed: %s", strerror(errno));
        return;
    }
}

File::~File()
{
    if(fd != -1)
    {
        close(fd);
    }
}

int64_t File::size() const
{
    if(fd != -1) [[likely]]
    {
        struct stat statbuf{};
        if(fstat(fd, &statbuf) == 0) [[likely]]
        {
            return statbuf.st_size;
        }
        return -1;
    }
    return -1;
}

size_t File::write(const unsigned char* data, size_t size)
{
}

size_t File::read(unsigned char* data, size_t size)
{
}

bool File::Exists(const char* path)
{
    return access(path, F_OK) == 0;
}


} // namespace tpunkt