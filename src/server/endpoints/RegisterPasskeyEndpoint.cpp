// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <sodium/randombytes.h>
#include "server/DTO.h"
#include "server/Endpoints.h"

namespace tpunkt
{

void RegisterPasskeyEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    if(RegisterRequest(res, req))
    {
        return;
    }

    res->onData(
        [ res ](std::string_view data, const bool last)
        {
            if(!last)
            {
                EndRequest(res, 431, "Sent data too large");
                return;
            }

            DTOPasskeyStart pubKey;
            randombytes_buf(pubKey.challenge, TPUNKT_CRYPTO_KEY_LEN);

        });

    res->onAborted([ res ]() { EndRequest(res, 500, "Server Error"); });
}

} // namespace tpunkt