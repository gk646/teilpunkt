#ifndef TPUNKT_CONFIG_H
#define TPUNKT_CONFIG_H

// Crypto

#define TPUNKT_CRYPTO_SESSION_LEN 32

// Authentication


// Storage

// Maximum name length for any file or storage endpoint
#define TPUNKT_STORAGE_NAME_LEN 56

// Sever

// Size of static file buffer
#define TPUNKT_SERVER_STATIC_FILES_LEN 25

// Directory that contains all static files to preload
#define TPUNKT_SERVER_STATIC_FILES_DIR "../static"

// Server Port - HTTPS port
#define TPUNKT_SERVER_PORT 8080

// Monitoring

#define TPUNKT_MONITOR_REQ_PER_MIN 50

template <typename T>
struct SensitiveContainer;

#endif //TPUNKT_CONFIG_H