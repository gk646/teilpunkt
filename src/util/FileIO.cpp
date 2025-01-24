#include <unistd.h>
#include "util/FileIO.h"

namespace tpunkt
{

    static bool FileExists( const char* path )
    {
        return access( path, F_OK ) == 0;
    }



} // namespace tpunkt