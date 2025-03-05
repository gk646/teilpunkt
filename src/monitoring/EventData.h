// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_EVENT_DATA_H
#define TPUNKT_EVENT_DATA_H

#include "fwd.h"

namespace tpunkt
{

struct FileSystemEventData final
{
    FileID file;
    // FileName name;
};

} // namespace tpunkt

#endif // TPUNKT_EVENT_DATA_H