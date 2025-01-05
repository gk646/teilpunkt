#include "server/WebServer.h"

static WebServer SERVER;

WebServer& GetWebServer() { return SERVER; }