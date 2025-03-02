
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
# Include Directories
# ------------------------------

target_include_directories(teilpunkt PRIVATE
        src
        external/uWebSocket/src
        external/uWebSocket/uSockets/src
)

# ------------------------------
# Compiler Definitions
# ------------------------------

target_compile_definitions(teilpunkt PRIVATE
        UWS_HTTPRESPONSE_NO_WRITEMARK
        # LIBUS_USE_QUIC
        TPUNKT_VERSION="${PROJECT_VERSION}"
)