// SPDX-License-Identifier: GPL-3.0-only
#include "server/Endpoints.h"
#include "server/WebServer.h"
#include "util/Logging.h"

namespace tpunkt
{

namespace
{
void CreateApp(uWS::SSLApp& app)
{
    // Authentication
    app.post("/api/auth/password", AuthPasswordEndpoint::handle);
    //    app.post("/api/auth/passkey-get", AuthPasskeyGetEndpoint::handle);
    // app.post("/api/auth/passkey", AuthPasskeyValidateEndpoint::handle);

    // Register
    app.post("/api/register/password", RegisterPasswordEndpoint::handle);
    //    app.get("/api/register/passkey-get", RegisterPasskeyOptionsEndpoint::handle);
    app.post("/api/register/passkey", RegisterPasskeyEndpoint::handle);

    //  Files
    app.post("/api/filesystem/file", FileCreateEndpoint::handle);
    app.del("/api/filesystem/file", FileDeleteEndpoint::handle);
    app.post("/api/filesystem/upload", FileUploadEndpoint::handle);
    app.get("/api/filesystem/download", FileDownloadEndpoint::handle);

    // Dirs
    app.post("/api/filesystem/directory", DirCreateEndpoint::handle);
    app.del("/api/filesystem/directory", DirDeleteEndpoint::handle);

    // Filesystem
    app.get("/api/filesystem/roots", FileRootsEndpoint::handle);

    // Misc
    app.get("/*", StaticEndpoint::handle);
}
} // namespace

// Translate it through a function we can friend
uWS::SSLApp& GetNextHandler()
{
    return GetWebServer().getNextHandler();
}

void WebThreadFunc(int threadNum, uWS::SocketContextOptions* options, volatile bool* isStarted)
{
    auto& app = GetWebServer().webThreads[ threadNum ].app;
    app = new uWS::SSLApp(*options);

    CreateApp(*app);
    app->listen(TPUNKT_SERVER_PORT,
                [ threadNum, isStarted ](const us_listen_socket_t* socket)
                {
                    if(socket)
                    {
                        LOG_INFO("Started WebServer Thread %d ", threadNum);
                    }
                    else
                    {
                        *isStarted = false;
                        LOG_INFO("Failed to start WebServer Thread %d ", threadNum);
                    }
                });

    // Only takes a plain function ptr - cannot capture anything
    app->preOpen(
        [](struct us_socket_context_t* context, LIBUS_SOCKET_DESCRIPTOR fd) -> LIBUS_SOCKET_DESCRIPTOR
        {
            (void)context;
            auto& receivingApp = GetNextHandler();
            receivingApp.getLoop()->defer([ fd, &receivingApp ]() { receivingApp.adoptSocket(fd); });
            return -1;
        });
    app->run();
    delete app;
}

namespace global
{
WebServer* WebServer;
}

WebServer::WebServer(const int threads) : staticFiles(TPUNKT_INSTANCE_STATIC_FILES_DIR), threadCount(threads)
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(WebServer);
    webThreads.reserve(threads + 1);

    uWS::SocketContextOptions options{
        .key_file_name = TPUNKT_INSTANCE_KEY_DIR, .cert_file_name = TPUNKT_INSTANCE_CERT_DIR, .passphrase = "1234"};

    volatile bool isStarted = true;
    for(int i = 0; i < threads; ++i)
    {
        auto& back = webThreads.emplace_back(WebThread{.thread = {}, .app = {}, .threadNum = i});
        back.thread = std::thread(WebThreadFunc, i, &options, &isStarted);
        usleep(100'000);
    }

    if(isStarted)
    {
        LOG_INFO("Server running at https://localhost:%d", TPUNKT_SERVER_PORT);
    }
    else
    {
        LOG_ERROR("Failed to start webserver");
    }
}

WebServer::~WebServer()
{
    TPUNKT_MACROS_GLOBAL_RESET(WebServer);
    for(auto& [ thread, app, num ] : webThreads)
    {
        app->close();
        if(thread.joinable())
        {
            thread.join();
        }
    }
}

const StaticFileStorage& WebServer::getStaticFileStorage()
{
    return staticFiles;
}

uWS::SSLApp& WebServer::getNextHandler()
{
    roundRobinNum = (roundRobinNum + 1) % threadCount;
    return *webThreads[ roundRobinNum ].app;
}

WebServer& GetWebServer()
{
    TPUNKT_MACROS_GLOBAL_GET(WebServer);
}


} // namespace tpunkt
