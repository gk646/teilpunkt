#include <httplib/httplib.h>

#include "server/Endpoints.h"

void DownloadEndpoint::handle(const httplib::Request& req, httplib::Response& res)
{
    auto password = req.get_param_value("password");
   // if (!authenticate(password))
    {
        res.status = 401; // Unauthorized
        res.set_content("Invalid password!", "text/plain");
        return;
    }

    auto filename = req.get_param_value("filename");
    std::ifstream ifs("./uploads/" + filename, std::ios::binary);
    if (ifs)
    {
        std::string file_content((std::istreambuf_iterator(ifs)), std::istreambuf_iterator<char>());
        res.set_content(file_content, "application/octet-stream");
        res.set_header("Content-Disposition", "attachment; filename=" + filename);
    }
    else
    {
        res.status = 404; // Not Found
        res.set_content("File not found!", "text/plain");
    }
}