cmake_minimum_required(VERSION 3.30)
project(teilpunkt)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(teilpunkt ${SOURCE_FILES})

target_include_directories(teilpunkt PRIVATE
        src
        external
        external/spdlog/include
)