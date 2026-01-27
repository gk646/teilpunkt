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
    StorageTransaction(ResultCb callback, uWS::HttpResponse<true>* response);
    TPUNKT_MACROS_MOVE_ONLY(StorageTransaction);
    virtual ~StorageTransaction() = default;

    void init(DataStore& store, VirtualFilesystem& vfs);

    // Commit the operation
    virtual void commit();

    [[nodiscard]] bool getIsValid() const;

  protected:
    uWS::HttpResponse<true>* response = nullptr;
    uWS::Loop* loop = nullptr;
    DataStore* datastore = nullptr;
    VirtualFilesystem* filesystem = nullptr;
    ResultCb callback;

    [[nodiscard]] bool shouldAbort() const;

  private:
    bool isCommited = false;
};

struct WriteFileTransaction final : StorageTransaction
{
    WriteFileTransaction(ResultCb callback, uWS::HttpResponse<true>* response, FileID dir, FileID file);
    ~WriteFileTransaction() override;

    bool start();
    void commit() override;
    bool write(const std::string_view& data, bool isLast);

  private:
    WriteHandle handle;
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
