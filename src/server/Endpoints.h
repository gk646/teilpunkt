// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_ENDPOINTS_H
#define TPUNKT_ENDPOINTS_H

#include "fwd.h"

namespace tpunkt
{

struct ServerEndpoint
{
    //===== Helper =====//

    // Returns request is authenticated
    static bool AuthRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, UserID& user);

    // Returns true if the request was already handled - internally calls all listeners and events for requests
    static bool RegisterRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void EndRequest(uWS::HttpResponse<true>* res, int code, const char* data = nullptr, bool close = false);

    static const char* GetHeader(uWS::HttpRequest* req, const char* keyName, size_t& length);
};

#define TPUNKT_MACROS_CHECK_REQUEST                                                                                    \
    if(RegisterRequest(res, req))                                                                                      \
    {                                                                                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    if(!AuthRequest(res, req, user))                                                                                   \
    {                                                                                                                  \
        return;                                                                                                        \
    }

struct AuthEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct UploadEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct DownloadEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct RegisterPasswordEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct RegisterPasskeyEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct StaticEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

} // namespace tpunkt

// namespace tpunkt
#endif // TPUNKT_ENDPOINTS_H