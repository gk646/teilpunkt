// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "server/Endpoints.h"
#include "storage/Storage.h"
#include "storage/StorageTransaction.h"
#include "server/DTOMappings.h"

namespace tpunkt
{

void FileDownloadEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    TPUNKT_MACROS_AUTH_USER()

    const FileID file = FileID::FromString(GetHeader(req, "file"));
    if(!file.isValid() || file.isDirectory())
    {
        EndRequest(res, 400, "Invalid file");
        return;
    }

    StorageEndpoint* endpoint = nullptr;
    auto status = Storage::GetInstance().endpointGet(user, file.getEndpoint(), endpoint);
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, "Invalid endpoint");
        return;
    }


    ResultCb callback = [ res ](bool success)
    {
        if(!success)
        {
            EndRequest(res, 400, "File write failed");
        }
    };
    auto transaction = std::make_shared<ReadFileTransaction>(callback, res, file);
    status = endpoint->fileRead(user, file, *transaction.get());
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

    DTO::ResponseDirectoryEntry info{};
    status = endpoint->infoFile(user, file, info);
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, GetStorageStatusStr(status));
        return;
    }

    res->writeHeader("Content-Disposition", std::string{"attachment; filename="} + std::string{info.name.view()});
    res->writeHeader("Content-Type", "application/octet-stream");
    res->writeHeader("Content-Length", std::to_string(info.sizeBytes));

    transaction->readFile();

    res->onWritable(
        [ transaction ](size_t) mutable -> bool
        {
            transaction->readFile();
            return true;
        });

    res->onAborted([ res ] { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt
