#ifndef TPUNKT_DATASTORE_H
#define TPUNKT_DATASTORE_H

#include <cstdint>
#include <functional>
#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{
    enum class DatastoreHandle : uint16_t;

    // Actual data interface - data is given and received encrypted only.
    // Only cares about data - doesn't know anything else - permissions etc. handled on layers above
    // Files are only identified by their ID from our side
    // This is an ASYNC interface - methods return true if the request was successful - actual result is callback
    // Note: Callback might be called before the method returns
    // Datastore's store their files in /endpoints/{id}/datastore
    struct DataStore
    {
        using ResultCallback = const std::function<void(bool success)>&;
        using ReadCallback = const std::function<void(const unsigned char* data, size_t size, bool success)>&;

        virtual ~DataStore() = default;

        virtual bool createFile(uint32_t fileID, ResultCallback callback) = 0;

        virtual bool deleteFile(uint32_t fileID, ResultCallback callback) = 0;

        virtual bool readFile(uint32_t fileID, ReadCallback callback) = 0;

        //===== Write =====//

        virtual bool initWrite(uint32_t fileID, DatastoreHandle& handle) = 0;

        // Internally handles out of order writes - either by rejecting or smoothing them
        virtual bool writeFile(DatastoreHandle handle, const unsigned char* data, size_t size, size_t idx,
                               ResultCallback callback) = 0;

        virtual bool commitWrite(DatastoreHandle handle) = 0;

        virtual bool closeWrite(DatastoreHandle handle) = 0;

      protected:
        explicit DataStore(EndpointID endpoint);
        EndpointID endpoint;
        FixedString<64> dir; // Directory of the datastore
    };

} // namespace tpunkt

#endif // TPUNKT_DATASTORE_H