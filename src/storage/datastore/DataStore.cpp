// SPDX-License-Identifier: GPL-3.0-only

#include <cerrno>
#include <charconv>
#include <cstdio>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/datastore/DataStore.h"
#include "util/Logging.h"

namespace tpunkt
{

bool ReadHandle::isValid() const
{
    return fd != -1 && fileID != 0 && buffer != UINT8_MAX;
}

bool ReadHandle::isDone() const
{
    if(end == 0)
    {
        return position == SIZE_MAX;
    }
    return position >= end;
}

bool WriteHandle::isValid() const
{
    return fileID != 0 && targetfd != -1 && tempfd != -1 && buffer != UINT8_MAX;
}

bool WriteHandle::isDone() const
{
    return done;
}

DataStore::DataStore(EndpointID endpoint)
{
    const auto endpointID = static_cast<int>(endpoint);
    if(snprintf(dir.data(), dir.capacity(), "%s/%d/%s", TPUNKT_STORAGE_ENDPOINT_DIR, endpointID,
                TPUNKT_STORAGE_DATASTORE_DIR) < 0)
    {
        LOG_CRITICAL("Failed to format datastore directory name");
    }

    if(mkdir(dir.data(), TPUNKT_INSTANCE_FILE_MODE) != 0 && errno != EEXIST)
    {
        LOG_CRITICAL("Failed to create datastore directory: %s", strerror(errno));
    }
}

} // namespace tpunkt