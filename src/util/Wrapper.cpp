// SPDX-License-Identifier: GPL-3.0-only

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include "config.h"
#include "util/Logging.h"
#include "util/Wrapper.h"

namespace tpunkt
{

bool CreateRelDir(const char* path, const bool canExist)
{
    if(mkdir(path, TPUNKT_INSTANCE_FILE_MODE) != 0)
    {
        if(canExist && errno == EEXIST)
        {
            return true;
        }
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        return false;
    }
    return true;
}

bool RemoveRelDir(const char* path)
{
    return rmdir(path) != 0;
}

size_t GetDiskSizeUsed()
{
    struct statvfs stat{};
    if(statvfs("/", &stat) != 0)
    {
        LOG_ERROR("Failed to get disk size of /:%s", strerror(errno));
        return 0;
    }
    const unsigned long available_space = stat.f_bavail * stat.f_frsize;
    return available_space;
}

int GetDiskUsage()
{
    struct statvfs stat{};
    if(statvfs("/", &stat) != 0)
    {
        LOG_ERROR("Failed to get disk size of /:%s", strerror(errno));
        return 0;
    }
    const unsigned long total_space = stat.f_blocks * stat.f_frsize;
    const unsigned long free_space = stat.f_bfree * stat.f_frsize;
    const unsigned long available_space = stat.f_bavail * stat.f_frsize;
    const unsigned long used_space = total_space - free_space;
    return static_cast<int>(static_cast<float>(used_space) / static_cast<float>(available_space) * 100.0F);
}

} // namespace tpunkt
