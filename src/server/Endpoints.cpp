// SPDX-License-Identifier: GPL-3.0-only

#include <HttpResponse.h>
#include <sodium/crypto_generichash.h>
#include "auth/Authenticator.h"
#include "monitoring/EventLimiter.h"
#include "server/DTOMappings.h"
#include "server/Endpoints.h"
#include "util/Strings.h"

namespace tpunkt
{

static const char* GetStatusString(const int status)
{
    switch(status)
    {
        case 100:
            return "100 Continue";
        case 101:
            return "101 Switching Protocols";
        case 102:
            return "102 Processing";
        case 103:
            return "103 Early Hints";

        case 200:
            return "200 OK";
        case 201:
            return "201 Created";
        case 202:
            return "202 Accepted";
        case 203:
            return "203 Non-Authoritative Information";
        case 204:
            return "204 No Content";
        case 205:
            return "205 Reset Content";
        case 206:
            return "206 Partial Content";
        case 207:
            return "207 Multi-Status";
        case 208:
            return "208 Already Reported";
        case 226:
            return "226 IM Used";

        case 300:
            return "300 Multiple Choices";
        case 301:
            return "301 Moved Permanently";
        case 302:
            return "302 Found";
        case 303:
            return "303 See Other";
        case 304:
            return "304 Not Modified";
        case 305:
            return "305 Use Proxy";
        case 307:
            return "307 Temporary Redirect";
        case 308:
            return "308 Permanent Redirect";

        case 400:
            return "400 Bad Request";
        case 401:
            return "401 Unauthorized";
        case 402:
            return "402 Payment Required";
        case 403:
            return "403 Forbidden";
        case 404:
            return "404 Not Found";
        case 405:
            return "405 Method Not Allowed";
        case 406:
            return "406 Not Acceptable";
        case 407:
            return "407 Proxy Authentication Required";
        case 408:
            return "408 Request Timeout";
        case 409:
            return "409 Conflict";
        case 410:
            return "410 Gone";
        case 411:
            return "411 Length Required";
        case 412:
            return "412 Precondition Failed";
        case 413:
            return "413 Payload Too Large";
        case 414:
            return "414 URI Too Long";
        case 415:
            return "415 Unsupported Media Type";
        case 416:
            return "416 Range Not Satisfiable";
        case 417:
            return "417 Expectation Failed";
        case 418:
            return "418 I'm a teapot";
        case 421:
            return "421 Misdirected Request";
        case 422:
            return "422 Unprocessable Entity";
        case 423:
            return "423 Locked";
        case 424:
            return "424 Failed Dependency";
        case 425:
            return "425 Too Early";
        case 426:
            return "426 Upgrade Required";
        case 428:
            return "428 Precondition Required";
        case 429:
            return "429 Too Many Requests";
        case 431:
            return "431 Request Header Fields Too Large";
        case 451:
            return "451 Unavailable For Legal Reasons";

        case 500:
            return "500 Internal Server Error";
        case 501:
            return "501 Not Implemented";
        case 502:
            return "502 Bad Gateway";
        case 503:
            return "503 Service Unavailable";
        case 504:
            return "504 Gateway Timeout";
        case 505:
            return "505 HTTP Version Not Supported";
        case 506:
            return "506 Variant Also Negotiates";
        case 507:
            return "507 Insufficient Storage";
        case 508:
            return "508 Loop Detected";
        case 510:
            return "510 Not Extended";
        case 511:
            return "511 Network Authentication Required";

        default:
            return "Unknown Status Code";
    }
}

bool ServerEndpoint::SessionAuth(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, UserID& user)
{
    size_t tokenLen = 0;
    const char* tokenStr = nullptr;
    size_t userLen = 0;
    const char* userStr = nullptr;
    uint32_t lookupUser{};

    // Delete both if either one is missing for consistency
    if((tokenStr = GetHeader(req, TPUNKT_AUTH_SESSION_ID_NAME, tokenLen)) == nullptr ||
       (userStr = GetHeader(req, TPUNKT_AUTH_SESSION_USER_NAME, userLen)) == nullptr ||
       !StringToNumber(userStr, userLen, lookupUser))
    {
        EndRequest(res, 400, "", false,
                   [](uWS::HttpResponse<true>* res)
                   {
                       ClearCookie(res, TPUNKT_AUTH_SESSION_ID_NAME);
                       ClearCookie(res, TPUNKT_AUTH_SESSION_USER_NAME);
                   });
        return false;
    }

    const SessionToken token{tokenStr, tokenLen};

    SessionMetaData metaData;
    if(!GetMetaData(res, req, metaData))
    {
        EndRequest(res, 400);
        return false;
    }

    const auto status = GetAuthenticator().sessionAuth(UserID{lookupUser}, token, metaData, user);
    if(status != AuthStatus::OK)
    {
        EndRequest(res, 401, GetAuthStatusStr(status));
        return false;
    }
    return true;
}

bool ServerEndpoint::AllowRequest(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    return GetEventLimiter().allowRequest(res, req);
}

void ServerEndpoint::EndRequest(uWS::HttpResponse<true>* res, const int code, const char* data, const bool close,
                                const ResponseFunc func)
{
    res->writeStatus(GetStatusString(code));
    res->writeHeader("Strict-Transport-Security", "max-age=31536000; includeSubDomains;");
    res->writeHeader("X-Frame-Options", "DENY");
    res->writeHeader("X-Content-Type-Options", "nosniff");
    res->writeHeader("Referrer-Policy", "strict-origin-when-cross-origin");
    res->writeHeader("Content-Security-Policy", "default-src 'none'; "
                                                "script-src 'self' 'wasm-unsafe-eval';"
                                                "style-src 'self';"
                                                "media-src 'self';"
                                                "connect-src 'self';"
                                                "img-src 'self'; ");
    res->writeHeader("X-XSS-Protection", "1; mode=block");
    res->writeHeader("Permissions-Policy", "publickey-credentials-get=(self)"
                                           "geolocation=(), microphone=(),camera=(),"
                                           "payment=(), usb=(),"
                                           "fullscreen=(self), autoplay=(self)");
    if(func != nullptr) [[unlikely]]
    {
        func(res);
    }
    res->end(data, close);
}


const char* ServerEndpoint::GetHeader(uWS::HttpRequest* req, const char* keyName, size_t& length)
{
    for(const auto& [ key, value ] : *req)
    {
        if(keyName == key)
        {
            length = value.size();
            return value.data();
        }
    }
    length = 0;
    return nullptr;
}

bool ServerEndpoint::GetMetaData(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, SessionMetaData& metaData)
{
    size_t agentLen = 0;
    const auto* agentStr = GetHeader(req, "user-agent", agentLen);
    if(agentStr == nullptr)
    {
        return false;
    }

    metaData.userAgent = UserAgentString{agentStr, agentLen};

    const auto& ipAddr = res->getRemoteAddress();
    metaData.remoteAddress = HashedIP{ipAddr.data(), ipAddr.size()};
    unsigned char* content = (unsigned char*)metaData.remoteAddress.data();
    constexpr size_t len = metaData.remoteAddress.capacity();
    crypto_generichash(content, len, content, len, nullptr, 0);
    return true;
}

void ServerEndpoint::SetCookie(uWS::HttpResponse<true>* res, const char* key, const char* value,
                               const uint32_t expiration)
{
    char buf[ 128 ];
    constexpr auto* fmt = "%s=%s; Path=/;HttpOnly;Secure;SameSite=Strict;Max-Age=%d";
    const auto result = snprintf(buf, 128, fmt, key, value, expiration);
    if(result < 0)
    {
        LOG_ERROR("Failed to set cookie:%s", key);
        return;
    }
    res->writeHeader("Set-Cookie", buf);
}

void ServerEndpoint::SetUnsafeCookie(uWS::HttpResponse<true>* res, const char* key, const char* value,
                                     const uint32_t expiration)
{
    char buf[ 128 ];
    constexpr auto* fmt = "%s=%s; Path=/;Secure;SameSite=Strict;Max-Age=%d";
    const auto result = snprintf(buf, 128, fmt, key, value, expiration);
    if(result < 0)
    {
        LOG_ERROR("Failed to set cookie:%s", key);
        return;
    }
    res->writeHeader("Set-Cookie", buf);
}

void ServerEndpoint::ClearCookie(uWS::HttpResponse<true>* res, const char* key)
{
    char buf[ 128 ];
    constexpr auto* fmt = "%s=; Path=/;Secure;SameSite=Strict;Max-Age=0";
    const auto result = snprintf(buf, 128, fmt, key);
    if(result < 0)
    {
        LOG_ERROR("Failed to clear cookie:%s", key);
        return;
    }
    res->writeHeader("Set-Cookie", buf);
}


} // namespace tpunkt