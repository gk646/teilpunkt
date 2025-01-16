#include <uWebSocket/src/App.h>
#include "server/Endpoints.h"
#include "datastructures/STFTable.h"
#include "auth/Authenticator.h"

namespace tpunkt
{
    void SignupEndpoint::handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
    {
        
        res->onData(
            [ res, req ](std::string_view data, bool last)
            {
                TextParser<{"username", 16}, {"password", 32}> stfTable;
                if(!stfTable.parse("username=Hey\npassword=123\n"))
                {
                    res->writeHeader("Content-Type", "text/plain");
                    res->writeStatus("400");
                    res->end(uWS::httpErrorResponses[ uWS::HTTP_ERROR_400_BAD_REQUEST ]);
                }
                else
                {
                    Credentials credentials{};
                    credentials.type = CredentialsType::PASSWORD;
                    credentials.password.assign("123");

                    UserName name{};
                    name.assign("Hey");

                    GetAuthenticator().userAdd(name, credentials);
                    res->end();
                }
            });

        res->onAborted([]()
        {

        });
    }

} // namespace tpunkt