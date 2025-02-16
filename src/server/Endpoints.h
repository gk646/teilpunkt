// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_ENDPOINTS_H
#define TPUNKT_ENDPOINTS_H

namespace uWS
{
struct HttpRequest;
template <bool T>
struct HttpResponse;
} // namespace uWS

namespace tpunkt
{

struct ServerEndpoint
{
    //===== Helper =====//

    // Returns request is authenticated
    static bool AuthRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    // Returns true if the request was already handled - internally calls all listeners and events for requests
    static bool HandleRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    static void RejectRequest(uWS::HttpResponse<true>* res, int code, const char* reason);
};

struct LoginEndpoint final : ServerEndpoint
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

struct SignupEndpoint final : ServerEndpoint
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