// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STORAGE_TRANSACTION_H
#define TPUNKT_STORAGE_TRANSACTION_H

#include "crypto/WrappedKey.h"
#include "fwd.h"
#include "storage/datastore/DataStore.h"
#include "util/Macros.h"

namespace tpunkt
{

struct StorageTransaction
{
    explicit StorageTransaction(DataStore& store, const WrappedKey& key);
    explicit StorageTransaction() = default;
    virtual ~StorageTransaction();

    // Commit the operation
    void commit();

    // Immediately try to abort
    void abort();

    [[nodiscard]] bool isFinished() const;
    // Once set cannot be reset
    void setFinished();

    [[nodiscard]] bool isPaused() const;
    void setPaused(bool val);

    uWS::HttpResponse<true>* response = nullptr;
    uWS::Loop* loop = nullptr;

  private:
    WrappedKey key;             // Copied key
    FileID file{};
    DataStore* store = nullptr; // Always valid
    bool isCommited = false;
    bool isAborted = false;
    bool finished = false;
    TPUNKT_MACROS_MOVE_ONLY(StorageTransaction);
};

struct CreateTransaction final : StorageTransaction
{
    CreateTransaction() = default;


  private:
    TPUNKT_MACROS_MOVE_ONLY(CreateTransaction);
};

struct ReadTransaction final : StorageTransaction
{
    ReadTransaction() = default;
    ~ReadTransaction() override;

    bool readFile(size_t chunkSize, ReadCb callback)
    {
        store->readFile(handle, )
    }

  private:
    ReadHandle handle;
    TPUNKT_MACROS_MOVE_ONLY(ReadTransaction);
};

} // namespace tpunkt

#endif // TPUNKT_STORAGE_TRANSACTION_H