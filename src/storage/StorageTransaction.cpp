// SPDX-License-Identifier: GPL-3.0-only

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
    : StorageTransaction(store), info(info), system(&system), parent(&parent)
{
}

CreateFileTransaction::~CreateFileTransaction()
{
    if(shouldAbort())
    {
        if(!parent->fileRemove(idx))
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

bool CreateFileTransaction::create(ResultCb callback)
{
    uint32_t idx;
    if(!parent->fileAdd(info, idx))
    {
        return false;
    }
    return datastore->createFile(info.id.file, callback);
}

} // namespace tpunkt