cmake_minimum_required(VERSION 3.30)
project(teilpunkt VERSION 0.0.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(teilpunkt ${SOURCE_FILES})

# ------------------------------
# Build Options
# ------------------------------

target_compile_options(teilpunkt PRIVATE
        -Wall               # Enable most warnings
        -Wextra             # Enable extra warnings
        -Wpedantic          # Enforce strict ISO compliance
        -Wconversion        # Warn on implicit conversions that may lose data
        -Wsign-conversion   # Warn on implicit signed/unsigned conversions
        #   -Wunused            # Warn on unused variables, parameters, functions, etc.
        -Wuninitialized     # Warn on use of uninitialized variables
        -Wnon-virtual-dtor  # Warn when a class with virtual methods has a non-virtual destructor
        -Woverloaded-virtual # Warn when a function is hidden by a virtual function of a base
        -Wduplicated-cond   # Warn on duplicated conditions in `if`/`else if` chains
        -Wduplicated-branches # Warn on duplicated branches in `if`/`else if` chains
        #    -Wnull-dereference  # Warn if a null pointer is dereferenced
        -Wformat=2          # Enable stricter format string checking
        -Wimplicit-fallthrough # Warn on implicit fallthrough in switch statements
        -Wcast-align         # Warn on potential performance issues with casted pointer alignment
        -Wmissing-include-dirs # Warn if include directories are missing
)

# Strip binary to minimize information (use for security-sensitive applications)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,-strip-all -flto=auto")
    set(CMAKE_CXX_FLAGS "-O2 -fno-plt -s -DNDEBUG -flto=auto -ffunction-sections -fdata-sections")
    set(CMAKE_C_FLAGS "-O2 -fno-plt -s -DNDEBUG -flto=auto -ffunction-sections -fdata-sections")
endif ()

# ------------------------------
# Custom Target: Build uSockets
# ------------------------------

# Add a custom target to build uSockets with libuv and OpenSSL
add_custom_target(build_usockets ALL
        COMMAND make default -C ${CMAKE_CURRENT_SOURCE_DIR}/external/uWebSocket WITH_LIBUV=1 WITH_OPENSSL=1
        COMMENT "Building uSockets with libuv & OpenSSL..."
)

# Add uSockets as an imported library
add_library(usockets STATIC IMPORTED)
set_target_properties(usockets PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/external/uWebSocket/uSockets/uSockets.a"
)

# Ensure uSockets is built before linking
add_dependencies(usockets build_usockets)

# ------------------------------
# Dependencies
# ------------------------------

find_library(SODIUM_LIBRARY sodium REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(libuv REQUIRED)
find_package(ZLIB REQUIRED)

# ------------------------------
# Include Directories
# ------------------------------

target_include_directories(teilpunkt PRIVATE
        src
        external
        external/uWebSocket/uSockets/src
)

# ------------------------------
# Linking Libraries
# ------------------------------

target_link_libraries(teilpunkt PRIVATE
        uv               # libuv
        ZLIB::ZLIB       # Zlib
        sodium           # libsodium
        OpenSSL::SSL     # OpenSSL
        ${CMAKE_CURRENT_SOURCE_DIR}/external/uWebSocket/uSockets/uSockets.a # uSockets
)

# ------------------------------
# Compiler Definitions
# ------------------------------

target_compile_definitions(teilpunkt PRIVATE
        UWS_HTTPRESPONSE_NO_WRITEMARK
        # LIBUS_USE_QUIC
        TPUNKT_VERSION="${PROJECT_VERSION}"
)

# ------------------------------
# Summary Output
# ------------------------------

add_subdirectory(tests)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")