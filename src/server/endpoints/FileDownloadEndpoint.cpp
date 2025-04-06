// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include "server/Endpoints.h"
#include "storage/Storage.h"
#include "storage/StorageTransaction.h"

namespace tpunkt
{

void FileDownloadEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    const size_t fileSize = 1 * 1024 * 1024; // 10 MB

    res->writeHeader("Content-Disposition", "attachment; filename=\"dummy.bin\"");
    res->writeHeader("Content-Type", "application/octet-stream");
    res->writeHeader("Content-Length", std::to_string(fileSize));

    std::string dummyChunk(TPUNKT_SERVER_CHUNK_SIZE, 'x');

    auto offset = 0;

    // Initial direct writes until backpressure occurs
    while(offset < fileSize)
    {
        size_t remaining = fileSize - offset;
        size_t toSend = std::min(TPUNKT_SERVER_CHUNK_SIZE, remaining);
        bool canWrite = res->write(std::string_view(dummyChunk.data(), toSend));
        offset += toSend;
        if(!canWrite)
        {
            break;
        }
    }

    if(offset >= fileSize)
    {
        res->end();
    }

    res->onWritable(
        [ res, fileSize, dummyChunk, offset ](size_t) mutable -> bool
        {
            while(offset < fileSize)
            {
                size_t remaining = fileSize - offset;
                size_t toSend = std::min(TPUNKT_SERVER_CHUNK_SIZE, remaining);
                bool canWrite = res->write(std::string_view(dummyChunk.data(), toSend));
                offset += toSend;
                if(!canWrite)
                {
                    // Backpressure: wait for next onWritable event
                    return false;
                }
            }
            res->end();
            return true;
        });

    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt