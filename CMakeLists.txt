cmake_minimum_required(VERSION 3.30)
project(teilpunkt)

set(CMAKE_CXX_STANDARD 20)

add_executable(teilpunkt src/main.cpp)


target_include_directories(teilpunkt PRIVATE
        external
        external/spdlog/include
)