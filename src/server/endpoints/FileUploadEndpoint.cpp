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
    status = endpoint->fileCreate(user, directory, creationInfo);
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, GetStorageStatusStr(status));
        return;
    }

    auto transaction = std::make_shared<WriteFileTransaction>();
    status = endpoint->fileWrite(user, directory, *transaction.get());
    if(status != StorageStatus::OK)
    {
        EndRequest(res, 400, GetStorageStatusStr(status));
        return;
    }

    const auto handlerFunc = [ transaction, res ](std::string_view data, const bool isLast) mutable
    {

        transaction.get()->print("onData callback called.");
        if(file->is_open())
        {
            file->write(chunk.data(), chunk.size());
        }

        if(isLast)
        {
            printf("Is last true\n");
            file->close();
            EndRequest(res, 200);
        }
    };

    res->onData(handlerFunc);

    res->onAborted(
        [ res, transaction ]() mutable
        {
            transaction.get()->print("onAborted callback called.");
            if(file->is_open())
            {
                file->close();
                std::remove("./uploaded_file.txt");
            }
            std::cerr << "Upload aborted" << std::endl;
            EndRequest(res, 400);
        });

    transaction.get()->print("handle method end.");
}

} // namespace tpunkt
