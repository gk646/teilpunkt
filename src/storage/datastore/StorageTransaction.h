// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_STORAGE_TRANSACTION_H
#define TPUNKT_STORAGE_TRANSACTION_H

namespace tpunkt
{
    struct StorageTransaction
    {
        StorageTransaction()
        {
        }
    };


    struct StorageTransactionAdd final
    {
        ~StorageTransactionAdd();
    };

} // namespace tpunkt

#endif // TPUNKT_STORAGE_TRANSACTION_H