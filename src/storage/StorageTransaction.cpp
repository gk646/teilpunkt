// SPDX-License-Identifier: Apache License 2.0

#include <HttpResponse.h>
#include "storage/StorageTransaction.h"
#include "vfs/VirtualDirectory.h"

namespace tpunkt
{

StorageTransaction::StorageTransaction(DataStore& store) : datastore(&store), isStarted(true)
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

bool StorageTransaction::shouldAbort() const
{
    return !isFinished() || isAborted;
}

CreateFileTransaction::CreateFileTransaction(DataStore& store, VirtualDirectory& parent, VirtualFilesystem& system,
                                             const FileCreationInfo& info)
    : StorageTransaction(store), info(info), system(&system), parent(&parent),
      guard(CooperativeSpinlockGuard{parent.lock, true})
{
}

CreateFileTransaction::~CreateFileTransaction()
{
    if(shouldAbort())
    {
        if(!parent->removeFile(info.id))
        {
            LOG_WARNING("File should not be gone");
        }
        auto cb = [ & ](const bool success)
        {
            if(!success)
            {
                // TODO requeue task - make sure to delete
                LOG_ERROR("Failed to delete file - requeuing");
            }
            else
            {
                auto endFunc = [ & ] { response->end(success ? "Abort success" : "Abort failed"); };
                loop->defer(endFunc);
            }
        };
        datastore->deleteFile(info.id.file, cb);
    }
    else
    {
        response->end("Success");
    }
}

bool CreateFileTransaction::create(ResultCb callback) const
{
    if(!parent->addFile(info))
    {
        return false;
    }
    return datastore->createFile(info.id.file, callback);
}

} // namespace tpunkt