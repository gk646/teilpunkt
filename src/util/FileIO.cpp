// SPDX-License-Identifier: Apache License 2.0

#include <unistd.h>
#include "util/FileIO.h"

namespace tpunkt
{

    static bool FileExists( const char* path )
    {
        return access( path, F_OK ) == 0;
    }

FileReader::FileReader(const char* path) {

       fd = openat(dirfd, buf, O_CREAT | O_RDWR, TPUNKT_INSTANCE_FILE_MODE);
        if(fd == -1) [[unlikely]]
        {
            LOG_ERROR("Creating file failed: %s", strerror(errno));
            return;
        }
    }


FileReader::~FileReader(){


    }

size_t FileReader::write(const unsigned char* data, size_t size){
    nonce = randombytes_buf();
        crypto_secretbox_easy
    }


size_t FileReader::read(unsigned char* data, size_t size){

    //TODO finish secret box calls
    //TODO refactor into ciphers
    }


} // namespace tpunkt