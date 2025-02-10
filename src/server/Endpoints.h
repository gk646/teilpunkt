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
    static bool isClientAuthenticated(uWS::HttpRequest* req);
    static void rejectRequest(uWS::HttpResponse<true>* res, int status);
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