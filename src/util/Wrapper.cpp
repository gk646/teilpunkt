// SPDX-License-Identifier: Apache License 2.0

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
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

} // namespace tpunkt