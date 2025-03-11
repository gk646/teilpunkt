// SPDX-License-Identifier: GPL-3.0-only

#include <App.h>
#include "auth/Authenticator.h"
#include "server/Endpoints.h"

namespace tpunkt
{
void SignupEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    TPUNKT_MACROS_CHECK_REQUEST;

    res->writeStatus("400");
    res->end("hey");
    return;

    res->onData(
        [ res, req ](std::string_view data, bool last)
        {
            if(false)
            {
                res->writeHeader("Content-Type", "text/plain");
                res->writeStatus("400");
                res->end(uWS::httpErrorResponses[ uWS::HTTP_ERROR_400_BAD_REQUEST ]);
            }
            else
            {
                Credentials credentials{};
                credentials.type = CredentialsType::PASSWORD;
                credentials.password = "123";

                UserName name{};
                name = "hey";

                GetAuthenticator().userAdd(name, credentials);
                res->end();
            }
        });

    res->onAborted([]()
        {

        });
}

} // namespace tpunkt