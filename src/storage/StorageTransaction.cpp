// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "storage/StorageTransaction.h"

namespace tpunkt
{

StorageTransaction::StorageTransaction(ResultCb callback, uWS::HttpResponse<true>* response)
    : response(response), loop(uWS::Loop::get()), callback(callback)
{
}

void StorageTransaction::init(DataStore& store, VirtualFilesystem& vfs)
{
    datastore = &store;
    filesystem = &vfs;
}

void StorageTransaction::commit()
{
    isCommited = true;
}

bool StorageTransaction::getIsValid() const
{
    return datastore != nullptr;
}

bool StorageTransaction::shouldAbort() const
{
    return !isCommited || !getIsValid();
}

} // namespace tpunkt
