#ifndef TPUNKT_STORAGE_H
#define TPUNKT_STORAGE_H

#include <cstdint>
#include <vector>

#include "storage/StorageEndpoint.h"

namespace tpunkt
{

    struct Storage final
    {
        std::vector<StorageEndpoint> endpoints;
    };

    Storage& GetStorage();

} // namespace tpunkt

#endif //TPUNKT_STORAGE_H