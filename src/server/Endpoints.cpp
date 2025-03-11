// SPDX-License-Identifier: GPL-3.0-only

#include "server/Endpoints.h"

namespace tpunkt
{
bool ServerEndpoint::AuthRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    return false;
}

bool ServerEndpoint::HandleRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    return false;
}

void ServerEndpoint::RejectRequest(uWS::HttpResponse<true>* res, int code, const char* reason)
{
}

} // namespace tpunkt