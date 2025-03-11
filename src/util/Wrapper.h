// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_WRAPPER_H
#define TPUNKT_WRAPPER_H

namespace tpunkt
{

// Creates the dir with the given relative path
bool CreateRelDir(const char* path, bool canExist);

bool RemoveRelDir(const char* path);

} // namespace tpunkt


#endif // TPUNKT_WRAPPER_H