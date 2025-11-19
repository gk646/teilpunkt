// SPDX-License-Identifier: GPL-3.0-only

#include "instance/MainThread.h"
#include "util/Logging.h"

namespace tpunkt
{

namespace global
{
static MainThread* MainThread;
}

MainThread::MainThread()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(MainThread);
}

MainThread::~MainThread()
{
    TPUNKT_MACROS_GLOBAL_RESET(MainThread);
}

MainThread& GetMainThread()
{
    TPUNKT_MACROS_GLOBAL_GET(MainThread);
}

int MainThread::run()
{
    while(running)
    {
        usleep(10000);
    }
    return 0;
}

void MainThread::stop()
{
    running = false;
}


} // namespace tpunkt
