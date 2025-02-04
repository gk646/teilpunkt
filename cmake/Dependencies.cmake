find_library(SODIUM_LIBRARY sodium REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(libuv REQUIRED)
find_package(ZLIB REQUIRED)


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



target_link_libraries(teilpunkt PRIVATE
        uv               # libuv
        ZLIB::ZLIB       # Zlib
        sodium           # libsodium
        OpenSSL::SSL     # OpenSSL
        ${CMAKE_CURRENT_SOURCE_DIR}/external/uWebSocket/uSockets/uSockets.a # uSockets
)