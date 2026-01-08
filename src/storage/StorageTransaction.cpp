// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "storage/StorageTransaction.h"
#include "storage/vfs/VirtualFilesystem.h"
#include "vfs/VirtualDirectory.h"

namespace tpunkt
{

StorageTransaction::StorageTransaction(DataStore& store) : datastore(&store)
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
    return !isFinished || !isCommited;
}

CreateFileTransaction::CreateFileTransaction(DataStore& store, VirtualFilesystem& system, const FileCreationInfo& info,
                                             const FileID dir)
    : StorageTransaction(store), info(info), system(&system), dir(dir)
{
}

CreateFileTransaction::~CreateFileTransaction()
{
    if(shouldAbort())
    {
        VirtualDirectory* createDir = system->getDir(dir);
        if(createDir == nullptr)
        {
            LOG_WARNING("Failed to revert transaction: Directory already deleted");
        }
        else if(!createDir->fileDelete(file))
        {
            LOG_WARNING("Failed to revert transaction: Filed already deleted");
        }

        const auto callback = [ & ](const bool success)
        {
            if(!success)
            {
                // TODO requeue task - make sure to delete
                LOG_ERROR("Failed to revert transaction: Datastore failed to remove file");
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
        datastore->deleteFile(file.getUID(), callback);
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

bool CreateFileTransaction::start(ResultCb callback)
{
    return datastore->createFile(file.getUID(), callback);
}

} // namespace tpunkt
