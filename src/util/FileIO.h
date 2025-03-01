// SPDX-License-Identifier: Apache License 2.0

#ifndef TPUNKT_FILEIO_H
#define TPUNKT_FILEIO_H

#include "util/Macros.h"

namespace tpunkt
{

    struct File final
    {
        File(const char* path);
        ~File();

        size_t size() const;

        size_t write(const unsigned char* data, size_t size);
        size_t read(unsigned char* data, size_t size);

      private:
        int fd = -1;
        uint32_t nonce = 0;
        TPUNKT_MACROS_STRUCT( File );
    };


    struct FileStream final
    {


      private:
        const char* path;
        TPUNKT_MACROS_STRUCT( FileStream );
    };


} // namespace tpunkt

#endif // TPUNKT_FILEIO_H