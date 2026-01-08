// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_EVENT_DATA_H
#define TPUNKT_EVENT_DATA_H

#include "common/FileID.h"
#include "fwd.h"

namespace tpunkt
{

struct APIRequestEventData final
{
};

struct AuthenticationEventData final
{
};

struct FilesystemEventData final
{
    FileID file;
    // FileName name;
};

struct SessionsEventData final
{
};

struct InstanceEventData final
{
};

struct ServerEventData final
{
};


} // namespace tpunkt

#endif // TPUNKT_EVENT_DATA_H
