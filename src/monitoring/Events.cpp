// SPDX-License-Identifier: GPL-3.0-only

#include "monitoring/Events.h"

const char* tpunkt::GetEventActionStr(EventAction action)
{
    switch(action)
    {
        case EventAction::INVALID:
            return "INVALID";
        // User
        case EventAction::UserAdd:
            return "UserAdd";
        case EventAction::UserLogin:
            return "UserLogin";
        case EventAction::UserRemove:
            return "UserRemove";
        case EventAction::UserChangeCredentials:
            return "UserChangeCredentials";
        case EventAction::UserAddTOTP:
            return "UserAddTOTP";
        // UserData
        case EventAction::UserDataGetName:
            return "UserDataGetName";
        case EventAction::UserDataGetWrappedKey:
            return "UserDataGetWrappedKey";
        // Session
        case EventAction::SessionAdd:
            return "SessionAdd";
        case EventAction::SessionRemove:
            return "SessionRemove";
        case EventAction::SessionAuthenticate:
            return "SessionAuthenticate";
        case EventAction::SessionGetSessions:
            return "SessionGetSessions";
        case EventAction::SessionGetInfo:
            return "SessionGetInfo";
        // Token
        case EventAction::TokenInvalidate:
            return "TokenInvalidate";
        // Endpoint
        case EventAction::StorageEndpointCreate:
            return "StorageEndpointCreate";
        case EventAction::StorageEndpointCreateFrom:
            return "StorageEndpointCreateFrom";
        case EventAction::StorageEndpointGet:
            return "StorageEndpointGet";
        case EventAction::StorageEndpointDelete:
            return "StorageEndpointDelete";
        // Virtual File System
        case EventAction::FileSystemFileCreate:
            return "FileSystemFileCreate";
        case EventAction::FilesystemFileWrite:
            return "FilesystemFileWrite";
        case EventAction::FilesystemFileRemove:
            return "FilesystemFileRemove";
        case EventAction::FilesystemDirCreate:
            return "FilesystemDirCreate";
        case EventAction::FilesystemDirRead:
            return "FilesystemDirRead";
        // TaskManager
        case EventAction::ThreadAdd:
            return "ThreadAdd";
        case EventAction::ThreadRemove:
            return "ThreadRemove";
        default:
            return "UNKNOWN";
    }
}
