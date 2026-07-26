cmake_minimum_required(VERSION 3.30)

include(cmake/GetCPM.cmake)

file(READ "${CMAKE_SOURCE_DIR}/version.txt" PROJECT_VERSION_RAW)
string(STRIP "${PROJECT_VERSION_RAW}" PROJECT_VERSION_RAW)

project(Nexus
    VERSION ${PROJECT_VERSION_RAW}
    LANGUAGES C CXX
    DESCRIPTION "Cross-platform graphics and audio engine"
    HOMEPAGE_URL "https://github.com/Bipto/Nexus")

message(STATUS "Configuring Nexus version ${PROJECT_VERSION}")

# ------------------------------------------------------------
# MSVC: Use embedded debug info (/Z7) instead of /Zi
# This is the correct modern CMake way (CMake 3.25+)
# ------------------------------------------------------------
if (MSVC)
    # Apply to Debug and RelWithDebInfo
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
        CACHE STRING "" FORCE)

    # Additional MSVC flags
    add_compile_options(/EHsc)

    # Optional: profiling
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /PROFILE")
endif()

# ------------------------------------------------------------
# sccache integration
# ------------------------------------------------------------
find_program(SCCACHE_PROGRAM sccache)
if (SCCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   "${SCCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
    message(STATUS "Sccache found: ${SCCACHE_PROGRAM}")
else()
    message(STATUS "Sccache not found")
endif()

# ------------------------------------------------------------
# Global options
# ------------------------------------------------------------

# Allow flexible page sizes on Android
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")

# Required for shared/static compatibility
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPM_DOWNLOAD_ALL ON)
set(CPM_USE_NAMED_CACHE_DIRECTORIES ON)

# Disable C++20 module scanning
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-int-conversion")
endif()

if (MINGW)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
endif()

add_definitions(-DTRACY_ENABLE)

# ------------------------------------------------------------
# Testing
# ------------------------------------------------------------
if (DEFINED BUILD_TESTING)
    enable_testing()
    include(CTest)

    CPMAddPackage(
        googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG 6986c2b
    )
endif()

# ------------------------------------------------------------
# Benchmarking
# ------------------------------------------------------------
if (DEFINED BUILD_BENCHMARKING)
    CPMAddPackage(
        benchmark
        GITHUB_REPOSITORY google/benchmark
        GIT_TAG v1.9.5
        OPTIONS
            "BENCHMARK_ENABLE_TESTING OFF"
    )
endif()


# ------------------------------------------------------------
# Subdirectories
# ------------------------------------------------------------
add_subdirectory(src/Nexus)

if (DEFINED NX_BUILD_EDITOR)
    add_subdirectory(src/Editor)
endif()

if (DEFINED NX_BUILD_DEMO)
    add_subdirectory(src/Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()

# Disable sccache for Assimp if it was fetched by CPM
if (TARGET assimp)
    get_target_property(assimp_source_dir assimp SOURCE_DIR)
    get_filename_component(assimp_dir "${assimp_source_dir}" DIRECTORY)

    message(STATUS "Disabling sccache for Assimp in: ${assimp_dir}")

    set_property(DIRECTORY "${assimp_dir}" PROPERTY RULE_LAUNCH_COMPILE "")
    set_property(DIRECTORY "${assimp_dir}" PROPERTY RULE_LAUNCH_LINK "")
endif()
