/*#include <httplib/httplib.h>
#include "server/Endpoints.h"

void UploadEndpoint::handle(const httplib::Request& req, httplib::Response& res)
{
    auto password = req.get_file_value("password").content;
   // if (!authenticate(password))
    {
        res.status = 401; // Unauthorized
        res.set_content("Invalid password!", "text/plain");
        return;
    }

    if (req.has_file("file"))
    {
        const auto& file = req.get_file_value("file");
        std::ofstream ofs("./uploads/" + file.filename, std::ios::binary);
        ofs.write(file.content.c_str(), file.content.size());
        ofs.close();
        res.set_content("File uploaded successfully!", "text/plain");
    }
    else
    {
        res.status = 400; // Bad httplib::Request
        res.set_content("No file uploaded!", "text/plain");
    }
}

*/