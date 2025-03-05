// SPDX-License-Identifier: GPL-3.0-only


#ifndef TPUNKT_INSTANCE_INFO_H
#define TPUNKT_INSTANCE_INFO_H


// Global Instance Info data

// TODO save global file descriptor for base dir -> for all openat()

#include "datastructures/SecureBox.h"

namespace tpunkt
{

struct InstanceInfo
{
    InstanceInfo();
    ~InstanceInfo();

    // Returns the file descriptor of the root start dir
    int getRoot() const;

    const SecureBox<InstanceSecret>& getInstanceSecret() const;

  private:
    int rootfd = -1;
    SecureBox<InstanceSecret> instanceSecret;
};


InstanceInfo& GetInstanceInfo();

} // namespace tpunkt

#endif // TPUNKT_INSTANCE_INFO_H