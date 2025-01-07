#include <httplib/httplib.h>
#include "server/Endpoints.h"

#include <spdlog/spdlog.h>


void SignupEndpoint::handle(const httplib::Request& req, httplib::Response& res)
{
    res.status = httplib::OK_200;
}