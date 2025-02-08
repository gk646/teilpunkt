#include <cerrno>
#include <charconv>
#include <cstdio>
#include <sys/stat.h>
#include "datastructures/FixedString.h"
#include "storage/datastore/DataStore.h"
#include "util/Logging.h"

namespace tpunkt
{

    DataStore::DataStore(const EndpointID endpoint) : endpoint(endpoint)
    {
        const int endpointNum = static_cast<int>(endpoint);
        if(snprintf(dir.data(), dir.capacity(), "%s/%d/%s", TPUNKT_STORAGE_ENDPOINT_DIR, endpointNum,
                    TPUNKT_STORAGE_DATASTORE_DIR) < 0)
        {
            LOG_CRITICAL("Failed to format datastore directory name");
        }

        if(mkdir(dir.data(), TPUNKT_INSTANCE_FILE_MODE) != 0 || errno != EEXIST)
        {
            LOG_CRITICAL("Failed to create datastore directory");
        }
    }

} // namespace tpunkt