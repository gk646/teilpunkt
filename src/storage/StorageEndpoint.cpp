// SPDX-License-Identifier: Apache License 2.0

#include <cerrno>
#include <cstdio>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/StorageEndpoint.h"
#include "util/Logging.h"

namespace tpunkt
{

StorageEndpoint::StorageEndpoint(const StorageEndpointCreateInfo& info, EndpointID eid, UserID creator, bool& success)
    : virtualFilesystem({})
{
    if(!success)
    {
        return;
    }

    if(mkdir(TPUNKT_STORAGE_ENDPOINT_DIR, TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
    {
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        success = false;
        return;
    }

    const auto endpointID = static_cast<int>(eid);
    if(snprintf(dir.data(), dir.capacity(), "%s/%d", TPUNKT_STORAGE_ENDPOINT_DIR, endpointID) < 0)
    {
        LOG_ERROR("Failed to format directory name");
        success = false;
        return;
    }

    if(mkdir(dir.c_str(), TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
    {
        LOG_ERROR("Failed to create endpoint directory:%s", strerror(errno));
        success = false;
        return;
    }
}

} // namespace tpunkt