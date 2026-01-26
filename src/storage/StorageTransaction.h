// SPDX-License-Identifier: GPL-3.0-only
#ifndef TPUNKT_STORAGE_TRANSACTION_H
#define TPUNKT_STORAGE_TRANSACTION_H

#include "fwd.h"
#include "storage/datastore/DataStore.h"
#include "util/Macros.h"
#include "vfs/VirtualFile.h"

namespace tpunkt
{

struct StorageTransaction
{
    explicit StorageTransaction(DataStore& store);
    TPUNKT_MACROS_MOVE_ONLY(StorageTransaction);
    virtual ~StorageTransaction() = default;

    // Commit the operation
    void commit();

    [[nodiscard]] bool getIsFinished() const;
    // Once set cannot be reset
    void setFinished();

    uWS::HttpResponse<true>* response = nullptr;
    uWS::Loop* loop = nullptr;

  protected:
    DataStore* datastore = nullptr; // Always valid

    [[nodiscard]] bool shouldAbort() const;

  private:
    bool isCommited = false;
    bool isFinished = false;
};

struct WriteFileTransaction final : StorageTransaction
{
    WriteFileTransaction(DataStore& store, VirtualFilesystem& system, const FileCreationInfo& info, FileID dir);
    ~WriteFileTransaction() override;

    // Queues the create-action
    bool start(ResultCb callback);

  private:
    FileCreationInfo info;
    VirtualFilesystem* system = nullptr;
    FileID dir;
    FileID file;
    TPUNKT_MACROS_STRUCT(WriteFileTransaction);
};

struct ReadFileTransaction final : StorageTransaction
{
    ReadFileTransaction() = default;
    ~ReadFileTransaction() override;

    bool readFile(size_t chunkSize, ReadCb callback)
    {
        // store->readFile(handle, )
    }

  private:
    ReadHandle handle;
    TPUNKT_MACROS_MOVE_ONLY(ReadFileTransaction);
};


} // namespace tpunkt

#endif // TPUNKT_STORAGE_TRANSACTION_H
