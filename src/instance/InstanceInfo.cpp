// SPDX-License-Identifier: GPL-3.0-only


#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "datastructures/FixedString.h"
#include "instance/InstanceInfo.h"
#include "util/Logging.h"
#include "util/Macros.h"

namespace tpunkt
{

namespace global
{
InstanceInfo* InstanceInfo = nullptr;

}

InstanceInfo::InstanceInfo()
{
    TPUNKT_MACROS_GLOBAL_ASSIGN(InstanceInfo);

    // Instance secret
    char buffer[ TPUNKT_INSTANCE_SECRET_MAX_LEN + 1 ];
    (void)fputs("Enter the instance secret:", stdout);
    const ssize_t bytesRead = read(STDIN_FILENO, buffer, TPUNKT_INSTANCE_SECRET_MAX_LEN);

    if(bytesRead == -1 || bytesRead == 0 || bytesRead > TPUNKT_INSTANCE_SECRET_MAX_LEN)
    {
        LOG_FATAL("Failed to read instance secret");
    }
    buffer[ bytesRead ] = '\0';

    auto reader = instanceSecret.get();
    reader.get() = buffer;

    // Startup dir
    rootfd = open("./", O_RDONLY | O_DIRECTORY);
    if(rootfd == -1)
    {
        LOG_FATAL("Failed to open startup dir:%s", strerror(errno));
    }
}

InstanceInfo::~InstanceInfo()
{
    TPUNKT_MACROS_GLOBAL_RESET(InstanceInfo);
    close(rootfd);
}

InstanceInfo& GetInstanceInfo()
{
    TPUNKT_MACROS_GLOBAL_GET(InstanceInfo);
}

int InstanceInfo::getRoot() const
{
    return rootfd;
}

const SecureBox<InstanceSecret>& InstanceInfo::getInstanceSecret() const
{
    return instanceSecret;
}


} // namespace tpunkt