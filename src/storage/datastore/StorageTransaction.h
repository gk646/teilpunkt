// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STORAGE_TRANSACTION_H
#define TPUNKT_STORAGE_TRANSACTION_H

#include "crypto/WrappedKey.h"

namespace tpunkt
{

struct StorageTransaction
{
    explicit StorageTransaction(DataStore& store, WrappedKey);
    virtual ~StorageTransaction() = default;

    virtual void commit() = 0;

    virtual void abort() = 0;

  private:
    DataStore& store;
    TPUNKT_MACROS_STRUCT(StorageTransaction);
};

struct CreateTransaction final
{

  private:
    TPUNKT_MACROS_STRUCT(CreateTransaction);
};

struct StorageTransactionAdd final
{
    ~StorageTransactionAdd();
};

} // namespace tpunkt

#endif // TPUNKT_STORAGE_TRANSACTION_H