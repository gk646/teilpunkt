#ifndef TPUNKT_FILEHANDLE_H
#define TPUNKT_FILEHANDLE_H

#include <cstdint>

namespace tpunkt
{

    // Abstract unique file identifier
    struct FileHandle final
    {
        uint32_t fileID;
    };
} // namespace tpunkt

#endif // TPUNKT_FILEHANDLE_H