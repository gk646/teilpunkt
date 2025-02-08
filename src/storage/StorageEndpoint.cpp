#include <cerrno>
#include <cstdio>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/StorageEndpoint.h"
#include "util/Logging.h"

namespace tpunkt
{
    StorageEndpoint::StorageEndpoint(const FileName& name, EndpointID endpoint) : virtualFilesystem({})
    {
        if(mkdir(TPUNKT_STORAGE_ENDPOINT_DIR, TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
        {
            LOG_ERROR("Failed to create endpoint directory");
        }

        if(snprintf(dir.data(), dir.capacity(), "%s/%d", TPUNKT_STORAGE_ENDPOINT_DIR, static_cast<int>(endpoint)) < 0)
        {
            LOG_ERROR("Failed to format directory name");
        }

        if(mkdir(dir.c_str(), TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
        {
            LOG_ERROR("Failed to create endpoint directory");
        }
    }

} // namespace tpunkt