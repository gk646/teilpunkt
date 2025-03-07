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

bool StorageTransaction::getIsFinished() const
{
    return isFinished;
}
void StorageTransaction::setFinished()
{
    isFinished = true;
}

bool StorageTransaction::shouldAbort() const
{
    return isStarted && (!isFinished || !isCommited);
}

CreateFileTransaction::CreateFileTransaction(DataStore& store, VirtualDirectory& parent, VirtualFilesystem& system,
                                             const FileCreationInfo& info, const FileID dir)
    : StorageTransaction(store), info(info), system(&system), parent(&parent), dir(dir)
{
}

CreateFileTransaction::~CreateFileTransaction()
{
    if(!isStarted)
    {
        LOG_ERROR("Transaction not started");
        response->end("Transaction not started");
    }
    else
    {
        if(shouldAbort())
        {
            (void)parent->fileRemove(file);

            auto cb = [ & ](const bool success)
            {
                if(!success)
                {
                    // TODO requeue task - make sure to delete
                    LOG_ERROR("Failed to delete file - requeuing");
                }
                else
                {
                    auto endFunc = [ & ]
                    {
                        response->writeStatus("500");
                        response->end();
                    };
                    loop->defer(endFunc);
                }
            };


            datastore->deleteFile(file.getBlock(), cb);
        }
        else
        {
            auto endFunc = [ & ]
            {
                response->writeStatus("200 OK");
                response->end();
            };
            loop->defer(endFunc);
        }
    }
}

bool CreateFileTransaction::start(ResultCb callback)
{
    isStarted = true;

    uint32_t idx;
    if(!parent->fileAdd(info, idx))
    {
        return false;
    }
    file = FileID{idx, dir.getEndpoint(), false};

    if(!datastore->createFile(idx, callback))
    {
        return false;
    }
    return true;
}

} // namespace tpunkt