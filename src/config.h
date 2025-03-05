// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_CONFIG_H
#define TPUNKT_CONFIG_H

//===== Crypto =====//

#define TPUNKT_CRYPTO_SESSION_ID_LEN 32

#define TPUNKT_CRYPTO_KEY_LEN 32

#define TPUNKT_INSTANCE_SECRET_MAX_LEN 32

//===== Authentication =====//


//===== Storage =====//

#define TPUNKT_STORAGE_ENDPOINT_DIR "./endpoints"

#define TPUNKT_STORAGE_DATASTORE_DIR "datastore"

// Maximum name length for any file or storage endpoint
#define TPUNKT_STORAGE_FILE_LEN 32

// Maximum length for usernames
#define TPUNKT_STORAGE_USER_LEN 16

// Max concurrent readers per datastore
#define TPUNKT_STORAGE_DATASTORE_MAX_READERS 25

// Default limit for the count of files and directories EACH across all endpoints
#define TPUNKT_STORAGE_MAX_DEFAULT_FILE_DIR_LIMIT 50'000

//===== Server =====//

#define TPUNKT_SERVER_CHUNK_SIZE 32000

// Size of static file buffer
#define TPUNKT_SERVER_STATIC_FILES_LEN 25

// Directory that contains all static files to preload
#define TPUNKT_SERVER_STATIC_FILES_DIR "../static"

// Server Port - HTTPS port
#define TPUNKT_SERVER_PORT 8080

//===== Monitoring =====//

#define TPUNKT_MONITOR_REQ_PER_MIN 50

//===== Instance =====//

// Max length of any string config parameter
#define TPUNKT_INSTANCE_CONFIG_STRING_LEN 24

// Creation flags for folders and files - only owner can read/write
#define TPUNKT_INSTANCE_FILE_MODE 0600

#endif // TPUNKT_CONFIG_H