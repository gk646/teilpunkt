// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include "server/StaticFileStorage.h"
#include "util/Logging.h"
#include "util/Memory.h"

namespace tpunkt
{
static const char* getMimeType(const char* path)
{
    constexpr struct
    {
        const char* extension;
        const char* mimeType;
    } mimeTypes[] = {{.extension = ".html", .mimeType = "text/html"},
                     {.extension = ".css", .mimeType = "text/css"},
                     {.extension = ".js", .mimeType = "application/javascript"},
                     {.extension = ".png", .mimeType = "image/png"},
                     {.extension = ".jpg", .mimeType = "image/jpeg"},
                     {.extension = ".jpeg", .mimeType = "image/jpeg"},
                     {.extension = ".gif", .mimeType = "image/gif"},
                     {.extension = ".svg", .mimeType = "image/svg+xml"},
                     {.extension = ".json", .mimeType = "application/json"},
                     {.extension = ".txt", .mimeType = "text/plain"},
                     {.extension = ".ico", .mimeType = "image/x-icon"},
                     {.extension = nullptr, .mimeType = "text/plain"}};

    const char* dot = strrchr(path, '.');
    if(dot == nullptr)
    {
        return mimeTypes[ 11 ].mimeType;
    }

    for(int i = 0; mimeTypes[ i ].extension != nullptr; ++i)
    {
        if(strcmp(dot, mimeTypes[ i ].extension) == 0)
        {
            return mimeTypes[ i ].mimeType;
        }
    }
    return mimeTypes[ 11 ].mimeType;
}

static void iterateDirectory(const char* dir, int& index, StaticFile* staticFiles)
{
    if(index == TPUNKT_SERVER_STATIC_FILES_LEN)
    {
        LOG_CRITICAL("Too many static files");
        return;
    }

    dirent* entry;
    DIR* dirFile = opendir(dir);
    constexpr int bufferSize = 512;

    if(dirFile == nullptr)
    {
        LOG_ERROR("Failed to open dir: %s", dir);
        return;
    }

    while((entry = readdir(dirFile)) != nullptr)
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char path[ bufferSize ]{};
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        if(strlen(path) > bufferSize)
        {
            LOG_CRITICAL("Static filepath too long");
            return;
        }

        struct stat statbuf{};
        if(stat(path, &statbuf) == 0)
        {
            if(S_ISDIR(statbuf.st_mode))
            {
                iterateDirectory(path, index, staticFiles);
            }
            else
            {
                FILE* file = fopen(path, "rx");
                if(file == nullptr)
                {
                    LOG_ERROR("Failed to load static file: %s", path);
                }

                // Name
                char* name = TPUNKT_ALLOC(name, strlen(path) - 8);
                strncpy(name, path + 9, strlen(path) - 8);

                // Content
                char* content = TPUNKT_ALLOC(content, static_cast<size_t>(statbuf.st_size) + 1u);
                (void)fread(content, static_cast<size_t>(statbuf.st_size), 1, file);
                content[ statbuf.st_size ] = '\0';
                (void)fclose(file);

                auto& staticFile = staticFiles[ index ];
                staticFile.size = static_cast<int>(statbuf.st_size);
                staticFile.name = name;
                staticFile.content = content;
                staticFile.type = getMimeType(staticFile.name);
                index++;
            }
        }
    }
    closedir(dirFile);
}

StaticFileStorage::StaticFileStorage(const char* directory)
{
    int index = 0;
    iterateDirectory(directory, index, staticFiles);
    LOG_INFO("Loaded %d static files", index);
}

StaticFileStorage::~StaticFileStorage()
{
    for(const auto& file : staticFiles)
    {
        if(file.name == nullptr)
        {
            break;
        }
        TPUNKT_FREE(file.content);
        TPUNKT_FREE(file.name);
    }
}

const StaticFile* StaticFileStorage::getFile(const char* filePath) const
{
    for(const auto& file : staticFiles)
    {
        if(file.name == nullptr)
        {
            return nullptr;
        }
        if(strcmp(file.name, filePath) == 0)
        {
            return &file;
        }
    }
    return nullptr;
}

} // namespace tpunkt