#ifndef TPUNKT_CONFIG_H
#define TPUNKT_CONFIG_H

// Crypto

#define TPUNKT_CRYPTO_SESSION_ID_LEN 32

// Authentication


// Storage

// Maximum name length for any file or storage endpoint
#define TPUNKT_STORAGE_NAME_LEN 56

// Maximum length for usernames
#define TPUNKT_STORAGE_USER_LEN 16


// Server

// Size of static file buffer
#define TPUNKT_SERVER_STATIC_FILES_LEN 25

// Directory that contains all static files to preload
#define TPUNKT_SERVER_STATIC_FILES_DIR "../static"

// Server Port - HTTPS port
#define TPUNKT_SERVER_PORT 8080


// Monitoring

#define TPUNKT_MONITOR_REQ_PER_MIN 50

// Instance

#define TPUNKT_INSTANCE_CONFIG_STRING_LEN 24


#endif // TPUNKT_CONFIG_H