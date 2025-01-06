#ifndef TPUNK_STORAGE_H
#define TPUNK_STORAGE_H

#include <cstdint>
#include <vector>

#include "storage/StorageEndpoint.h"

namespace tpunkt
{
    enum class StorageStatusType : uint8_t
    {
        INVALID = 0,
        OK = 1,
        ERR_NOT_INITIALIZED,
        ERR_NOT_ENOUGH_SPACE,
        ERR_ACTION_DENIED,
    };

    struct StorageStatus final
    {
        StorageStatusType status;
        StorageStatus(StorageStatusType status);
        bool isOK() const;
        const char* getErrorMessage() const;
    };

    struct Storage final
    {
        std::vector<StorageEndpoint> endpoints;
    };

    Storage& GetStorage();

} // namespace tpunkt

#endif //TPUNK_STORAGE_H