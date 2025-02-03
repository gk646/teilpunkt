#ifndef TPUNKT_DATASTORE_H
#define TPUNKT_DATASTORE_H

#include <cstdint>
#include "fwd.h"

namespace tpunkt
{
    // Actual datastore interface - data is only given and received encrypted
    // Only cares about data - doesn't know anything else - permissions etc. handled on layers above
    // Files are only identified by their ID from our side
    struct DataStore
    {
        virtual ~DataStore() = 0;

        // Creates the given file
        virtual bool createFile(uint32_t fileID) = 0;

        // Deletes the given file
        virtual bool deleteFile(uint32_t fileID) = 0;

        virtual bool exists() = 0;

        //
        virtual bool writeChunk();

        virtual bool readChunk();
    };


    struct FileSystemDataStore final : DataStore
    {
        bool createFile(uint32_t fileID) override;
        bool deleteFile(uint32_t fileID) override;
    };


} // namespace tpunkt

#endif // TPUNKT_DATASTORE_H