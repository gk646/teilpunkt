#ifndef TPUNKT_CONFIG_H
#define TPUNKT_CONFIG_H

// Crypto

#define TPUNKT_SESSION_ID_BYTES 32

// Authentication


// Storage

// Maximum name length for any file or storage endpoint
#define TPUNKT_STORAGE_NAME_LEN 56

// Monitoring


#define TPUNKT_REQUESTS_PER_MIN 50

template <typename T>
struct SensitiveContainer;

// Logging


#define LOG_INFO(msg, ...) spdlog::info(msg, #__VAR_ARGS__);
#define LOG_WARNING(msg, ...) spdlog::warn(msg, #__VAR_ARGS__);
#define LOG_ERROR(msg, ...) spdlog::error(msg, #__VAR_ARGS__);

#endif //TPUNKT_CONFIG_H