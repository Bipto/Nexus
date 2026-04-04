cmake_minimum_required(VERSION 3.30)

include(cmake/GetCPM.cmake)
include(cmake/DependencyManager.cmake)

file(READ "${CMAKE_SOURCE_DIR}/version.txt" PROJECT_VERSION_RAW)
string(STRIP "${PROJECT_VERSION_RAW}" PROJECT_VERSION_RAW)

project(Nexus
    VERSION ${PROJECT_VERSION_RAW}
    LANGUAGES C CXX
    DESCRIPTION "Cross-platform graphics and audio engine"
    HOMEPAGE_URL "https://github.com/Bipto/Nexus")

message(STATUS "Configuring Nexus version ${PROJECT_VERSION}")

#allow flexible page sizes on Android devices
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")

#required to make shared and static libs be compatible on some platforms
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CPM_DOWNLOAD_ALL ON)
set(CPM_USE_NAMED_CACHE_DIRECTORIES ON)

#disable cmake module scanning
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-int-conversion")
endif()

if (MINGW)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
endif()

if (MSVC)
    #enable exceptions for all targets (MSVC only)
    add_compile_options(/EHsc)

    #enable profiling support
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /PROFILE")
endif()

add_definitions(-DTRACY_ENABLE)

#testing
if (DEFINED BUILD_TESTING)
    enable_testing()
    include(CTest)
    include(FetchContent)

    FetchContentWithCache(
        googletest
        URL https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )

    find_package(GTest CONFIG REQUIRED)
endif()

#set up engine project
add_subdirectory(src/Nexus)

if (DEFINED NX_BUILD_DEMO)
    add_subdirectory(src/Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()