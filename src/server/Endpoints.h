// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_ENDPOINTS_H
#define TPUNKT_ENDPOINTS_H

#include "fwd.h"

namespace tpunkt
{

struct ServerEndpoint
{
    //===== Helper =====//

    // Returns true and assigns user if request has a valid session cookie
    static bool AuthRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, UserID& user);

    // Returns true if the request is allowed
    static bool AllowRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    // Ends the request
    static void EndRequest(uWS::HttpResponse<true>* res, int code, const char* data = "", bool close = false);

    // Returns nullptr if no header with the given name is found
    static const char* GetHeader(uWS::HttpRequest* req, const char* keyName, size_t& length);

    // Returns true if the connection metadata is successfully set
    static bool GetMetaData(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, SessionMetaData& metaData);

    // Sets the given cookie
    static void SetCookie(uWS::HttpResponse<true>* res, const char* key, const char* value, uint32_t expiration);
};

#define TPUNKT_MACROS_CHECK_REQUEST                                                                                    \
    if(!AllowRequest(res, req))                                                                                        \
    {                                                                                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    if(!AuthRequest(res, req, user))                                                                                   \
    {                                                                                                                  \
        EndRequest(res, 401);                                                                                          \
        return;                                                                                                        \
    }

struct AuthPasswordEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct AuthPasskeyEndpoint final : ServerEndpoint
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