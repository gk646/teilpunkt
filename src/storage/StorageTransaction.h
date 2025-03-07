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
    TPUNKT_MACROS_MOVE_ONLY(StorageTransaction);

  protected:
    bool isStarted = false;

  private:
    bool isCommited = false;
    bool isFinished = false;
};

struct CreateFileTransaction final : StorageTransaction
{
    explicit CreateFileTransaction(DataStore& store, VirtualDirectory& parent, VirtualFilesystem& system,
                                   const FileCreationInfo& info, FileID dir);
    ~CreateFileTransaction() override;

    // Queues the create-action
    bool start(ResultCb callback);

  private:
    FileCreationInfo info;
    VirtualFilesystem* system = nullptr;
    VirtualDirectory* parent;
    FileID dir;
    FileID file;
    TPUNKT_MACROS_STRUCT(CreateFileTransaction);
};

struct WriteFileTransaction final : StorageTransaction
{
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