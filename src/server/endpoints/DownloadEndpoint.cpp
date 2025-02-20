// SPDX-License-Identifier: Apache License 2.0

#include <uWebSocket/src/HttpResponse.h>
#include "server/Endpoints.h"
#include "storage/Storage.h"
#include "storage/StorageTransaction.h"

namespace tpunkt
{

namespace
{

bool sendData(uWS::HttpResponse<true>* res)
{
}

} // namespace

void DownloadEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(HandleRequest(res, req))
    {
        return;
    }

    if(!AuthRequest(res, req))
    {
        return;
    }

    size_t begin{};
    size_t end{};
    UserID user{};
    FileID file{}; // From request
    StorageEndpoint* endpoint = nullptr;

    {
        const auto ret = GetStorage().endpointGet(user, file.endpoint, endpoint);
        if(ret != StorageStatus::OK)
        {
            RejectRequest(res, StorageHTTPErrCode(ret), StorageErrString(ret));
            return;
        }
    }

    // TODO use slot allocator
    auto state = std::make_shared<ReadTransaction>(ReadTransaction{});
    {
        const auto ret = endpoint->fileGet(user, file, begin, end, *state);
        if(endpoint == nullptr || ret != StorageStatus::OK)
        {
            RejectRequest(res, StorageHTTPErrCode(ret), StorageErrString(ret));
            return;
        }
    }

    state->response = res;
    state->loop = uWS::Loop::get();


    auto dataCallback = [ state ](const unsigned char* data, size_t length, bool success, bool isLast)
    {
        if(!success || state->isFinished()) // If finished we shouldn't be here
        {
            state->abort();
            // Schedule termination on the event loop thread.
            state->loop->defer([ state ]() { state->response->end("Error reading file"); });
        }

        if(isLast)
        {
            state->setFinished();
        }

        auto sendData = [ data, length, state ]()
        {
            // Attempt to send chunk
            const auto dataChunk = std::string_view(reinterpret_cast<const char*>(data));
            const auto [ ok, done ] = state->response->tryEnd(dataChunk, length);
            if(done)
            {
                state->setFinished();
            }
            else if(!ok)
            {
                // Backpressure detected
                state->setPaused(true);
            }
        };
        state->loop->defer(sendData);
    };

    auto sendChunk = [ state, dataCallback ]()
    {
        if(state->isFinished() || state->isPaused())
        {
            return;
        }

        if(!state->readFile(TPUNKT_SERVER_CHUNK_SIZE, dataCallback))
        {
            state->abort();
            state->response->end("Error reading file");
        }
    };

    // Register writable callback
    res->onWritable(
        [ state, sendChunk ](size_t offset)
        {
            state->setPaused(false);
            sendChunk();
            return !state->isPaused();
        });

    res->onAborted([ state ] { state->abort(); });

    // Start streaming
    sendChunk();
}

} // namespace tpunkt