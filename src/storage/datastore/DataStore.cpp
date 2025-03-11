// SPDX-License-Identifier: GPL-3.0-only

#include <cerrno>
#include <cstdio>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/datastore/DataStore.h"
#include "util/Logging.h"
#include "util/Wrapper.h"

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
    const auto endpointNum = static_cast<int>(endpoint);
    (void)snprintf(dir.data(), dir.capacity(), "%s/%d/%s", TPUNKT_STORAGE_ENDPOINT_DIR, endpointNum,
                   TPUNKT_STORAGE_DATASTORE_DIR);
}

bool DataStore::CreateDirs(EndpointID endpoint)
{
    FixedString<64> dir;
    const auto endpointNum = static_cast<int>(endpoint);
    if(snprintf(dir.data(), dir.capacity(), "%s/%d/%s", TPUNKT_STORAGE_ENDPOINT_DIR, endpointNum,
                TPUNKT_STORAGE_DATASTORE_DIR) < 0)
    {
        LOG_CRITICAL("Failed to format datastore directory name");
    }

    if(CreateRelDir(dir.data(), true))
    {
        return false;
    }

    return true;
}

} // namespace tpunkt