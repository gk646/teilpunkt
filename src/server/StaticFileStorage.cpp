#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "server/StaticFileStorage.h"
#include "util/Logger.h"

namespace tpunkt
{
    static void iterateDirectory(const char* dir, int& index, StaticFile* staticFiles)
    {
        if (index == TPUNKT_SERVER_STATIC_FILES_LEN)
        {
            LOG_FATAL("Too many static files");
            return;
        }

        dirent* entry;
        DIR* dp = opendir(dir);

        if (dp == nullptr)
        {
            LOG_ERROR("Failed to open dir: %s", dir);
            return;
        }

        while ((entry = readdir(dp)) != nullptr)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            char path[512]{};
            snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

            if (strlen(path) > 512)
            {
                LOG_FATAL("Error iterating files");
                return;
            }

            struct stat statbuf{};
            if (stat(path, &statbuf) == 0)
            {
                if (S_ISDIR(statbuf.st_mode))
                {
                    iterateDirectory(path, index, staticFiles); // Recursive call
                }
                else
                {
                    FILE* file = fopen(path, "rx");
                    if (!file)
                    {
                        LOG_ERROR("Failed to load static file: %s", path);
                    }

                    // Name
                    auto name = (char*)malloc(strlen(path) - 8);
                    strncpy(name, path + 9, strlen(path) - 8);

                    // Content
                    auto content = (char*)malloc(statbuf.st_size + 1);
                    fread(content, statbuf.st_size, 1, file);
                    content[statbuf.st_size] = '\0';

                    auto& staticFile = staticFiles[index];
                    staticFile.size = statbuf.st_size;
                    staticFile.name = name;
                    staticFile.content = content;
                    index++;
                }
            }
        }
        closedir(dp);
    }

    StaticFileStorage::StaticFileStorage(const char* directory)
    {
        int index = 0;
        iterateDirectory(directory, index, staticFiles);
    }

    StaticFileStorage::~StaticFileStorage()
    {
        for (auto& file : staticFiles)
        {
            if (!file.name)
                break;
            free((void*)file.content);
            free((void*)file.name);
        }
    }

    const StaticFile* StaticFileStorage::getFile(const char* filePath) const
    {
        for (const auto& file : staticFiles)
        {
            if (file.name == nullptr)
                return nullptr;
            if (strcmp(file.name, filePath) == 0)
            {
                return &file;
            }
        }
        return nullptr;
    }

} // namespace tpunkt