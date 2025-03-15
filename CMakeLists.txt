cmake_minimum_required(VERSION 3.30)
project(teilpunkt VERSION 0.0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable(teilpunkt ${SOURCE_FILES})

# ------------------------------
# Config
# ------------------------------

option(TPUNKT_BUILD_TESTS "Build the tests" ON)


# Build options
include(cmake/BuildOptions.cmake)

# Add dependencies
include(cmake/Dependencies.cmake)

# Build tests
if(TPUNKT_BUILD_TESTS)
    add_subdirectory(tests)
endif()