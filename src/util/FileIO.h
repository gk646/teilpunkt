#ifndef TPUNKT_FILEIO_H
#define TPUNKT_FILEIO_H

#include "util/Macros.h"

namespace tpunkt
{

    struct FileSection
    {
    };

    struct FixedSection final : FileSection
    {
        FixedSection() = default;
    };

    struct FileSchema final
    {
      private:
        static constexpr int MAX_SCHEMAS = 5;
        FileSection schemas[ MAX_SCHEMAS ];
        TPUNKT_MACROS_STRUCT( FileSchema );
    };


    struct FileHeader final
    {
        TPUNKT_MACROS_STRUCT( FileHeader );
    };

    struct FileReader final
    {
        FileReader( const char* path );


      private:
        const char* path;
        TPUNKT_MACROS_STRUCT( FileReader );
    };


    struct FileWriter final
    {


      private:
        const char* path;
        TPUNKT_MACROS_STRUCT( FileWriter );
    };


} // namespace tpunkt

#endif // TPUNKT_FILEIO_H