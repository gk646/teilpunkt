// SPDX-License-Identifier: Apache License 2.0

#include "storage/StorageTransaction.h"

namespace tpunkt
{

StorageTransaction::StorageTransaction(DataStore& store, const WrappedKey& key)
{

}

StorageTransaction::~StorageTransaction()
{

}

void StorageTransaction::commit()
{
    isCommited = true;
}

void StorageTransaction::abort()
{
    isCommited = false;
}

} // namespace tpunkt