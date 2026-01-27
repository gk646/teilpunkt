// SPDX-License-Identifier: GPL-3.0-only
#include <HttpResponse.h>
#include "storage/StorageTransaction.h"
#include "storage/vfs/VirtualFilesystem.h"
#include "server/Endpoints.h"

namespace tpunkt
{

ReadFileTransaction::ReadFileTransaction(ResultCb callback, uWS::HttpResponse<true>* response, FileID file)
    : StorageTransaction(callback, response), file(file)
{
}

ReadFileTransaction::~ReadFileTransaction()
{
    if(shouldAbort())
    {
        datastore->closeRead(handle, callback);
    }
}

bool ReadFileTransaction::start()
{
    return datastore->initRead(file.getUID(), 0, 0, handle);
}

bool ReadFileTransaction::readFile()
{
    auto readCallback = [ & ](const unsigned char* data, size_t size, bool success, bool isLast)
    {
        if(!success)
        {
            ServerEndpoint::EndRequest(response, 400, "Read Transaction failed");
            return;
        }
        canReadMore = response->write(std::string_view{reinterpret_cast<const char*>(data), size});
        if(isLast)
        {
            canReadMore = false;
            datastore->closeRead(handle, callback);
            response->end();
            commit();
        }
    };

    while(canReadMore && datastore->readFile(handle, TPUNKT_STORAGE_FILE_CHUNK_SIZE, readCallback))
    {
    }
    return true;
}


} // namespace tpunkt
