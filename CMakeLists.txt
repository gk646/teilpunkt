cmake_minimum_required(VERSION 3.30)
project(teilpunkt)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(teilpunkt ${SOURCE_FILES})


add_custom_target(build_usockets ALL
        COMMAND make default -C external/uWebSocket WITH_LIBUV=1 WITH_OPENSSL=1
        COMMENT "Building uSockets with libuv & OpenSSL..."
)

add_library(usockets STATIC IMPORTED)

set_target_properties(usockets PROPERTIES
        IMPORTED_LOCATION "external/uWebSocket/uSockets/uSockets.a"
)

add_dependencies(usockets build_usockets)

find_library(SODIUM_LIBRARY sodium REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(libuv REQUIRED)
find_package(ZLIB REQUIRED)

target_include_directories(teilpunkt PRIVATE
        src
        external
        external/uWebSocket/uSockets/src
)

target_link_libraries(teilpunkt PRIVATE
        uv
        ZLIB::ZLIB
        sodium
        OpenSSL::SSL
        ${CMAKE_CURRENT_SOURCE_DIR}/external/uWebSocket/uSockets/uSockets.a)


target_compile_definitions(teilpunkt PRIVATE UWS_HTTPRESPONSE_NO_WRITEMARK LIBUS_USE_QUIC)