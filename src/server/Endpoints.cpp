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

bool ServerEndpoint::HasValidSession(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, UserID& user)
{
    const std::string_view tokenCookie = GetCookie(req, TPUNKT_AUTH_SESSION_TOKEN_NAME);
    const std::string_view userCookie = GetCookie(req, TPUNKT_AUTH_SESSION_USER_NAME);

    uint32_t lookupUser = 0;
    if(tokenCookie.empty() || userCookie.empty() || !StringToNumber(userCookie, lookupUser))
    {
        EndRequest(res, 400, "", false,
                   [](uWS::HttpResponse<true>* res) -> void
                   {
                       // Delete both if either one is missing for consistency
                       ClearCookie(res, TPUNKT_AUTH_SESSION_TOKEN_NAME);
                       ClearCookie(res, TPUNKT_AUTH_SESSION_USER_NAME);
                   });
        return false;
    }

    SessionMetaData metaData;
    if(!GetMetaData(res, req, metaData))
    {
        EndRequest(res, 400);
        return false;
    }

    const SessionToken token{tokenCookie};
    const AuthStatus status = Authenticator::GetInstance().sessionAuth(UserID{lookupUser}, token, metaData, user);
    if(status != AuthStatus::OK)
    {
        EndRequest(res, 401, Authenticator::GetStatusStr(status));
        return false;
    }
    return true;
}

bool ServerEndpoint::IsRateLimited(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)
{
    return GetEventLimiter().allowRequest(res, req);
}

bool ServerEndpoint::IsRequestEmpty(uWS::HttpResponse<true>* res, const std::string_view& data, bool isLast)
{
    (void)isLast;
    if(data.empty())
    {
        EndRequest(res, 400, "Empty request body");
        return true;
    }
    return false;
}

bool ServerEndpoint::IsRequestTooLarge(uWS::HttpResponse<true>* res, const std::string_view& data, bool isLast)
{
    (void)data;
    if(!isLast) // Too long
    {
        EndRequest(res, 431, "Sent data too large");
        return true;
    }
    return false;
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


std::string_view ServerEndpoint::GetHeader(uWS::HttpRequest* req, const char* key)
{
    for(const auto& [ keyName, value ] : *req)
    {
        if(keyName == key)
        {
            return value;
        }
    }
    return {};
}

std::string_view ServerEndpoint::GetCookie(uWS::HttpRequest* req, const char* name)
{
    std::string_view fullCookies = GetHeader(req, "cookie");
    if(fullCookies.empty())
    {
        return {};
    }
    const std::string_view nameSv(name);

    size_t start = 0;
    while(start < fullCookies.size())
    {
        const size_t nameEnd = fullCookies.find('=', start);
        if(nameEnd == std::string_view::npos) // Malformed cookie
        {
            break;
        }

        if(fullCookies.substr(start, nameEnd - start) == nameSv)
        {
            const size_t valueStart = nameEnd + 1;
            size_t valueEnd = fullCookies.find(';', valueStart);
            if(valueEnd == std::string_view::npos)
            {
                valueEnd = fullCookies.size();
            }
            return fullCookies.substr(valueStart, valueEnd - valueStart);
        }

        start = fullCookies.find(';', nameEnd) + 2;
        if(start == std::string_view::npos || start == 0) // No more cookies
        {
            break;
        }
    }

    // Cookie not found
    return {};
}

bool ServerEndpoint::GetMetaData(uWS::HttpResponse<true>* res, uWS::HttpRequest* req, SessionMetaData& metaData)
{
    const std::string_view agentStr = GetHeader(req, "user-agent");
    if(agentStr.empty())
    {
        return false;
    }

    metaData.userAgent = agentStr;
    metaData.remoteAddress = res->getRemoteAddress();

    // Hash the remote address to only store anonymized data (inplace)
    unsigned char* content = (unsigned char*)metaData.remoteAddress.data();
    const size_t len = metaData.remoteAddress.size();
    crypto_generichash(content, len, content, len, nullptr, 0);
    return true;
}

void ServerEndpoint::SetCookie(uWS::HttpResponse<true>* res, const char* key, const char* value,
                               const uint32_t expiration)
{
    char buf[ 256 ];
    constexpr const char* fmt = "%s=%s; Path=/;HttpOnly;Secure;SameSite=Strict;Max-Age=%d";
    const int result = snprintf(buf, 128, fmt, key, value, expiration);
    if(result < 0)
    {
        LOG_ERROR("Failed to format cookie:%s", key);
        return;
    }
    res->writeHeader("Set-Cookie", buf);
}

void ServerEndpoint::SetUnsafeCookie(uWS::HttpResponse<true>* res, const char* key, const char* value,
                                     const uint32_t expiration)
{
    char buf[ 256 ];
    constexpr const char* fmt = "%s=%s; Path=/;Secure;SameSite=Strict;Max-Age=%d";
    const int result = snprintf(buf, 128, fmt, key, value, expiration);
    if(result < 0)
    {
        LOG_ERROR("Failed to format cookie:%s", key);
        return;
    }
    res->writeHeader("Set-Cookie", buf);
}

void ServerEndpoint::ClearCookie(uWS::HttpResponse<true>* res, const char* key)
{
    char buf[ 128 ];
    constexpr const char* fmt = "%s=; Path=/;Secure;SameSite=Strict;Max-Age=0";
    const int result = snprintf(buf, 128, fmt, key);
    if(result < 0)
    {
        LOG_ERROR("Failed to format cookie:%s", key);
        return;
    }
    res->writeHeader("Set-Cookie", buf);
}


} // namespace tpunkt
