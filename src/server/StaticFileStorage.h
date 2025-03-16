// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_STATIC_FILESTORAGE_H
#define TPUNKT_STATIC_FILESTORAGE_H

#include "config.h"
#include "util/Macros.h"

namespace tpunkt
{
    struct StaticFile final
    {
        char* name = nullptr;
        char* content = nullptr;
        const char* type = nullptr;
        int size = 0;

        StaticFile() = default;
        TPUNKT_MACROS_STRUCT(StaticFile);
    };

    struct StaticFileStorage final
    {
        explicit StaticFileStorage(const char* directory);
        ~StaticFileStorage();

        const StaticFile* getFile(const char* filePath, size_t length) const;

      private:
        StaticFile staticFiles[ TPUNKT_SERVER_STATIC_FILES_LEN ]{};
        TPUNKT_MACROS_STRUCT(StaticFileStorage);
    };

} // namespace tpunkt

#endif // TPUNKT_STATIC_FILESTORAGE_H