// SPDX-License-Identifier: GPL-3.0-only
#include <cstring>
#include "common/FileID.h"
#include "datastructures/Spinlock.h"

namespace tpunkt
{

inline Spinlock ID_LOCK{};
inline uint32_t UID = 1;

FileID FileID::FromNum(uint64_t num)
{
    FileID fid{};
    memcpy(&fid, &num, sizeof(uint64_t));
    return fid;
}

uint64_t FileID::ToNum(FileID fid)
{
    uint64_t num{};
    memcpy(&num, &fid, sizeof(uint64_t));
    return num;
}

FileID::FileID(EndpointID endpoint, const bool isDirectory) : endpoint(endpoint), directory(isDirectory)
{
    SpinlockGuard lock{ID_LOCK};
    uid = UID++;
}

} // namespace tpunkt
