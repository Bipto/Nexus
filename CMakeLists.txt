cmake_minimum_required(VERSION 3.28)
cmake_policy(SET CMP0135 NEW)

project("Nexus")
include(FetchContent)

set(BUILD_SHARED_LIBS OFF)

#disable cmake module scanning
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

#allow flexible page sizes on Android devices
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")

#required to make shared and static libs be compatible on some platforms
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 6986c2b
    )
endif()

#set up engine project
add_subdirectory(src/Nexus)

if (DEFINED NX_BUILD_DEMO)
    add_subdirectory(src/Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()

if (DEFINED NX_BUILD_EDITOR)
    add_subdirectory(src/Editor)
endif()