#ifndef TPUNKT_STORAGE_H
#define TPUNKT_STORAGE_H

#include "storage/StorageEndpoint.h"
#include "datastructures/Spinlock.h"

namespace tpunkt
{
    // All operations are atomic
    struct Storage final
    {

      private:
        //===== Endpoint Management =====//


        std::vector<StorageEndpoint> endpoints;
        Spinlock storageLock;
        friend StorageTransaction;
    };

    Storage& GetStorage();

} // namespace tpunkt

#endif // TPUNKT_STORAGE_H