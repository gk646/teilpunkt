// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_DATASTORE_H
#define TPUNKT_DATASTORE_H

#include <cstdint>
#include <functional>
#include "datastructures/Buffer.h"
#include "datastructures/FixedString.h"
#include "fwd.h"

namespace tpunkt
{

struct ReadHandle final
{
    size_t position = 0;     // Current read/write position
    size_t end = 0;          // End position for reading
    uint32_t fileID = 0;
    int fd = -1;             // File descriptor (for open reads/writes)
    uint8_t buffer = UINT8_MAX;

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isDone() const;
};

struct WriteHandle final
{
    size_t newSize = 0;      // Total new size of the file
    size_t tempPosition = 0; // Pos in temp file
    uint32_t fileID = 0;
    int targetfd = -1;       // File for actual target
    int tempfd = -1;         // File where data is written to before commit
    uint8_t buffer = UINT8_MAX;
    bool done = false;

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isDone() const;
};

// Actual data interface - only cares about data - permissions etc. handled on layers above
// Notes:
//      - Files are only identified by their ID from our side
//      - Data is given and received encrypted only (assumed)
//      - Callback might be called before the method returns
//      - Datastores store their files in /endpoints/{id}/datastore
//      - Supports partial reads
//      - This is any ASYNC interface - return true is good request - callback true is good operation
//      - Operations are NOT threadsafe - must be enforced elsewhere
//      - Read/Writes are only consistent if close is called for every init
struct DataStore
{
    using ResultCb = const std::function<void(bool success)>&;
    // Data may be null if not success
    // isLast is true when callback will be called for the last time - either on error or finish
    using ReadCb = const std::function<void(const unsigned char* data, size_t size, bool success, bool isLast)>&;

    virtual ~DataStore() = default;

    virtual bool createFile(uint32_t fileID, ResultCb callback) = 0;

    virtual bool deleteFile(uint32_t fileID, ResultCb callback) = 0;

    //===== Read =====//

    // If end=0 means until end => 0/0 means read whole file
    // Handle is only valid if returns true
    virtual bool initRead(uint32_t fileID, size_t begin, size_t end, ReadHandle& handle) = 0;

    // Reads are guaranteed to be sequential
    virtual bool readFile(ReadHandle& handle, size_t chunkSize, ReadCb callback) = 0;

    virtual bool closeRead(ReadHandle& handle, ResultCb callback) = 0;

    //===== Write =====//

    // Handle is only valid if returns true
    virtual bool initWrite(uint32_t fileID, WriteHandle& handle) = 0;

    // Given data MUST be sequential (correct order)!
    virtual bool writeFile(WriteHandle& handle, bool isLast, const unsigned char* data, size_t size, ResultCb clb) = 0;

    virtual bool closeWrite(WriteHandle& handle, bool revert, ResultCb callback) = 0;

  protected:
    explicit DataStore(EndpointID endpoint);
    SyncedBuffer buffers[ TPUNKT_STORAGE_DATASTORE_MAX_READERS ]{}; // Cant use vector as expanding needs sync
    FixedString<64> dir;                                            // Directory of the datastore
    EndpointID endpoint;
};

} // namespace tpunkt

#endif // TPUNKT_DATASTORE_H