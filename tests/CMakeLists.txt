enable_testing()

file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

list(REMOVE_ITEM SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(teilpunkt-tests ${TEST_FILES} ${SOURCE_FILES})

target_include_directories(teilpunkt-tests PRIVATE
        ../src
        ../external/uWebSocket/uSockets/src
        ../external/uWebSocket/src
        ../external/ankerl/include
        ../external/glaze/include
        catch2
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(teilpunkt-tests PRIVATE
        UWS_HTTPRESPONSE_NO_WRITEMARK
        # LIBUS_USE_QUIC
        TPUNKT_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(teilpunkt-tests PRIVATE
        uv               # libuv
        ZLIB::ZLIB       # Zlib
        sodium           # libsodium
        OpenSSL::SSL     # OpenSSL
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/uWebSocket/uSockets/uSockets.a # uSockets
)