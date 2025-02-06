#ifndef TPUNKT_DATASTORE_H
#define TPUNKT_DATASTORE_H

#include <cstdint>
#include <functional>
#include "fwd.h"

namespace tpunkt
{
    using ResultCallback = std::function<void(bool success)>;

    // Actual datastore interface - data is given and received encrypted only.
    // Only cares about data - doesn't know anything else - permissions etc. handled on layers above
    // Files are only identified by their ID from our side
    struct DataStore
    {
        virtual ~DataStore() = 0;

        // Creates the given file
        virtual bool createFile(uint32_t fileID, ResultCallback callback) = 0;

        // Deletes the given file
        virtual bool deleteFile(uint32_t fileID, ResultCallback callback) = 0;

        //
        virtual bool writeChunk(const unsigned char* data, size_t len, ResultCallback callback);

        virtual bool readChunk(unsigned char* data, size_t len, ResultCallback callback);
    };


    struct FileSystemDataStore final : DataStore
    {

    };


} // namespace tpunkt

#endif // TPUNKT_DATASTORE_H