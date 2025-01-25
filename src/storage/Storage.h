#ifndef TPUNKT_STORAGE_H
#define TPUNKT_STORAGE_H

#include <cstdint>
#include <vector>

#include "storage/StorageEndpoint.h"

namespace tpunkt
{
    enum class StorageStatus : uint8_t
    {

    };

    struct Storage final
    {




      private:
        std::vector<StorageEndpoint> endpoints;
    };

    Storage& GetStorage();

} // namespace tpunkt

#endif // TPUNKT_STORAGE_H