// SPDX-License-Identifier: Apache License 2.0

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