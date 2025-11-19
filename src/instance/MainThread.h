// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_MAINTHREAD_H
#define TPUNKT_MAINTHREAD_H

#include <vector>
#include "server/WebServer.h"

namespace tpunkt
{

struct InstanceTask final
{
};

struct MainThread final
{
    MainThread();
    ~MainThread();

    int run();
    void stop();

  private:
    bool running = true;
};

MainThread& GetMainThread();


} // namespace tpunkt

#endif // TPUNKT_MAINTHREAD_H
