#ifndef TPUNKT_ENDPOINTS_H
#define TPUNKT_ENDPOINTS_H

namespace httplib
{
    struct Request;
    struct Response;
} // namespace httplib


struct LoginEndpoint final
{
    static void handle(const httplib::Request&, httplib::Response&);
};

struct UploadEndpoint final
{
    static void handle(const httplib::Request&, httplib::Response&);
};

struct DownloadEndpoint final
{
    static void handle(const httplib::Request&, httplib::Response&);
};

struct SignupEndpoint final
{
    static void handle(const httplib::Request&, httplib::Response&);
};


#endif //TPUNKT_ENDPOINTS_H