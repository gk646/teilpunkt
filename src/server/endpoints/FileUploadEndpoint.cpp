// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <storage/Storage.h>
#include "server/Endpoints.h"
#include "server/DTOMappings.h"
#include "storage/StorageTransaction.h"

namespace tpunkt
{

void FileUploadEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    TPUNKT_MACROS_AUTH_USER()

    const FileName fileName = GetHeader(req, "file-name");
    if(fileName.isEmpty() || !IsValidFilename(fileName))
    {
        EndRequest(res, 400, "Invalid file name");
        return;
    }

    const FileID directory = FileID::FromString(GetHeader(req, "directory"));
    if(!directory.isValid())
    {
        EndRequest(res, 400, "Invalid directory");
        return;
    }

    StorageEndpoint* endpoint = nullptr;
    auto status = Storage::GetInstance().endpointGet(user, directory.getEndpoint(), endpoint);
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, "Invalid endpoint");
        return;
    }

    FileCreationInfo creationInfo{.name = fileName, .creator = user, .endpoint = directory.getEndpoint()};
    FileID newFile{};
    status = endpoint->fileCreate(user, directory, creationInfo, newFile);
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, GetStorageStatusStr(status));
        return;
    }

    ResultCb callback = [ res ](bool success)
    {
        if(!success)
        {
            EndRequest(res, 400, "File write failed");
        }
    };

    auto transaction = std::make_shared<WriteFileTransaction>(callback, res, directory, newFile);
    status = endpoint->fileWrite(user, newFile, *transaction.get());
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, GetStorageStatusStr(status));
        return;
    }

    if(!transaction->getIsValid() || !transaction->start())
    {
        EndRequest(res, 400, "Failed to start transaction");
        return;
    }

    res->onData(
        [ transaction, res ](const std::string_view data, const bool isLast) mutable
        {
            if(!transaction->write(data, isLast))
            {
                EndRequest(res, 400, "Error writing file");
            }
            else if(isLast)
            {
                transaction->commit();
            }
        });
    res->onAborted([ res ] { EndRequest(res, 400); });
}

} // namespace tpunkt
