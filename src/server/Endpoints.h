// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_ENDPOINTS_H
#define TPUNKT_ENDPOINTS_H

#include <string_view>
#include "fwd.h"

namespace tpunkt
{

struct ServerEndpoint
{
    //===== Helper =====//

    // Returns true and assigns user if request has a valid session cookie
    // IFF false automatically ends the request with either 400 or 401
    static bool SessionAuth(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, UserID& user);

    // Returns true if the request is allowed
    static bool AllowRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);

    using ResponseFunc = void (*)(uWS::HttpResponse<true>* res);

    // Ends the request with the given parameters - optionally executes the given function before ending
    static void EndRequest(uWS::HttpResponse<true>* res, int code, const char* data = "", bool close = false,
                           ResponseFunc func = nullptr);

    // Returns nullptr if no header with the given name is found
    static const char* GetHeader(uWS::HttpRequest* req, const char* keyName, size_t& length);

    // Returns a view of the cookie if exists, else empty
    static std::string_view GetCookie(const std::string_view& cookieHeader, const char* name);

    // Returns true if the connection metadata is successfully set
    static bool GetMetaData(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, SessionMetaData& metaData);

    // Sets the cookie with the given value
    static void SetCookie(uWS::HttpResponse<true>* res, const char* key, const char* value, uint32_t expiration);

    // Sets the cookie such that it's accessible via clientside JavaScript
    static void SetUnsafeCookie(uWS::HttpResponse<true>* res, const char* key, const char* value, uint32_t expiration);

    // Clears the given cookie
    static void ClearCookie(uWS::HttpResponse<true>* res, const char* key);
};

//===== Auth =====//

struct AuthPasswordEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct AuthPasskeyGetEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct AuthPasskeyValidateEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

//===== Register =====//

struct RegisterPasswordEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct RegisterPasskeyOptionsEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct RegisterPasskeyEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

//===== Filesystem =====//

struct FileUploadEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct FileDownloadEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct FileDirectoryEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

struct FileRootsEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};

//===== Misc =====//


struct StaticEndpoint final : ServerEndpoint
{
    static void handle(uWS::HttpResponse<true>* res, uWS::HttpRequest* req);
};


} // namespace tpunkt

// namespace tpunkt
#endif // TPUNKT_ENDPOINTS_H