#ifndef TPUNKT_STATIC_FILESTORAGE_H
#define TPUNKT_STATIC_FILESTORAGE_H

#include "config.h"

namespace tpunkt
{

    struct StaticFile final
    {
        const char* name = nullptr;
        const char* content = nullptr;
        const char* type = nullptr;
        int size = 0;
    };

    struct StaticFileStorage final
    {
        StaticFile staticFiles[TPUNKT_SERVER_STATIC_FILES_LEN]{};

        explicit StaticFileStorage(const char* directory);
        ~StaticFileStorage();

        const StaticFile* getFile(const char* filePath) const;
    };

} // namespace tpunkt

#endif //TPUNKT_STATIC_FILESTORAGE_H