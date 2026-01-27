// SPDX-License-Identifier: GPL-3.0-only
#include <HttpResponse.h>
#include "storage/StorageTransaction.h"
#include "storage/vfs/VirtualFilesystem.h"

namespace tpunkt
{

WriteFileTransaction::WriteFileTransaction(ResultCb callback, uWS::HttpResponse<true>* response, FileID dir,
                                           FileID file)
    : StorageTransaction(callback, response), dir(dir), file(file)
{
}

WriteFileTransaction::~WriteFileTransaction()
{
    if(shouldAbort())
    {
        VirtualDirectory* createDir = filesystem->findDir(dir);
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
        datastore->closeWrite(handle, true, callback);
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

bool WriteFileTransaction::start()
{
    return datastore->createFile(file.getUID(), callback) && datastore->initWrite(file.getUID(), handle);
}

void WriteFileTransaction::commit()
{
    datastore->closeWrite(handle, false, callback);
    StorageTransaction::commit();
}

bool WriteFileTransaction::write(const std::string_view& data, bool isLast)
{
    return datastore->writeFile(handle, isLast, reinterpret_cast<const unsigned char*>(data.data()), data.size(),
                                callback);
}

} // namespace tpunkt
