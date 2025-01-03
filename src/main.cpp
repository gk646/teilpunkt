#include <httplib/httplib.h>
#define FMT_UNICODE 0

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>    // For file logging
#include <spdlog/sinks/stdout_color_sinks.h> // For console logging

#include "User.h"

void initialize_logger(bool logToFile = false)
{
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);

        std::vector<spdlog::sink_ptr> sinks = {console_sink};
        if (logToFile)
        {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("teilpunkt.log", true);
            file_sink->set_level(spdlog::level::debug);
            sinks.push_back(file_sink);
        }

        auto logger = std::make_shared<spdlog::logger>("", sinks.begin(), sinks.end());
        spdlog::set_default_logger(logger);     // Set as the default logger
        spdlog::set_level(spdlog::level::info); // Global log level
        spdlog::flush_on(spdlog::level::info);  // Flush logs on each info level log
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }

    spdlog::debug("spdlog initialized");
}

bool authenticate(const std::string& password) {
    const std::string valid_password = "securepassword";
    return password == valid_password;
}

void serve_static(httplib::Server& svr, const std::string& folder) {
    svr.set_mount_point("/", folder);
}

void handle_file_upload(const httplib::Request& req, httplib::Response& res) {
    auto password = req.get_file_value("password").content;
    if (!authenticate(password)) {
        res.status = 401; // Unauthorized
        res.set_content("Invalid password!", "text/plain");
        return;
    }

    if (req.has_file("file")) {
        const auto& file = req.get_file_value("file");
        std::ofstream ofs("./uploads/" + file.filename, std::ios::binary);
        ofs.write(file.content.c_str(), file.content.size());
        ofs.close();
        res.set_content("File uploaded successfully!", "text/plain");
    } else {
        res.status = 400; // Bad httplib::Request
        res.set_content("No file uploaded!", "text/plain");
    }
}

void handle_file_download(const httplib::Request& req,httplib:: Response& res) {
    auto password = req.get_param_value("password");
    if (!authenticate(password)) {
        res.status = 401; // Unauthorized
        res.set_content("Invalid password!", "text/plain");
        return;
    }

    auto filename = req.get_param_value("filename");
    std::ifstream ifs("./uploads/" + filename, std::ios::binary);
    if (ifs) {
        std::string file_content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        res.set_content(file_content, "application/octet-stream");
        res.set_header("Content-Disposition", "attachment; filename=" + filename);
    } else {
        res.status = 404; // Not Found
        res.set_content("File not found!", "text/plain");
    }
}

int main()
{
    initialize_logger(true);

    httplib::Server svr;
    svr.set_logger([](const httplib::Request& req, const httplib::Response& res)
                   { spdlog::info("Request:{}\nResponse:{}", req.body.c_str(), res.body.c_str()); });

    // Serve static files
    serve_static(svr, "../static");

    // Endpoints
    svr.Post("/upload", handle_file_upload);
    svr.Get("/download", handle_file_download);

    // Start server
    std::cout << "Server running at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    spdlog::shutdown();
    return 0;
}