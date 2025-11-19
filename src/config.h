// SPDX-License-Identifier: GPL-3.0-only

#ifndef TPUNKT_CONFIG_H
#define TPUNKT_CONFIG_H

#include <cstddef>

//===== Crypto =====//

constexpr size_t TPUNKT_CRYPTO_SESSION_ID_LEN = 32;

constexpr size_t TPUNKT_CRYPTO_KEY_LEN = 32;

constexpr size_t TPUNKT_INSTANCE_SECRET_MAX_LEN = 32;

//===== Authentication =====//

constexpr auto* TPUNKT_AUTH_SESSION_TOKEN_NAME = "tp-session-token";

constexpr auto* TPUNKT_AUTH_SESSION_USER_NAME = "tp-session-user";

constexpr size_t TPUNKT_AUTH_PASSWORD_LEN = 32;

constexpr size_t TPUNKT_AUTH_PASSKEY_LEN = 128;

constexpr size_t TPUNKT_AUTH_MIN_USER_LEN = 3;

//===== Storage =====//

constexpr auto* TPUNKT_STORAGE_ENDPOINT_DIR = "./endpoints";

constexpr auto* TPUNKT_STORAGE_DATASTORE_DIR = "datastore";

// Maximum name length for any file or storage endpoint
constexpr size_t TPUNKT_STORAGE_FILE_LEN = 64;

// Maximum length for usernames
constexpr size_t TPUNKT_STORAGE_USER_LEN = 16;

// Max concurrent readers per datastore
constexpr size_t TPUNKT_STORAGE_DATASTORE_MAX_READERS = 25;

// Default limit for the count of files and directories EACH across all endpoints
constexpr size_t TPUNKT_STORAGE_MAX_DEFAULT_FILE_DIR_LIMIT = 50'000U;

//===== Server =====//

constexpr size_t TPUNKT_SERVER_CHUNK_SIZE = 85'000;

// Size of static file buffer
constexpr size_t TPUNKT_SERVER_STATIC_FILES_LEN = 32;

// Server Port - HTTPS port
constexpr size_t TPUNKT_SERVER_PORT = 8080;

// Start size of the character buffer for returned json in bytes
constexpr size_t TPUNKT_SERVER_JSON_THREAD_BUFFER_START = 512;

//===== Monitoring =====//

constexpr size_t TPUNKT_MONITOR_REQ_PER_MIN = 50;

//===== Instance =====//

// Directory that contains all static files to preload
constexpr auto* TPUNKT_INSTANCE_STATIC_FILES_DIR = "../frontend";

// Path where the key SSL key is stored
constexpr auto* TPUNKT_INSTANCE_KEY_DIR = "../key/key.pem";

// Path where teh SSL certificate is stored
constexpr auto* TPUNKT_INSTANCE_CERT_DIR = "../key/cert.pem";

// Max length of any string config parameter
constexpr size_t TPUNKT_INSTANCE_CONFIG_STRING_LEN = 24;

// Creation flags for folders and files - only owner can read/write
#define TPUNKT_INSTANCE_FILE_MODE 0600

#endif // TPUNKT_CONFIG_H
