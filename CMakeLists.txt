cmake_minimum_required(VERSION 3.28)
cmake_policy(SET CMP0135 NEW)

project("Nexus")

set(BUILD_SHARED_LIBS OFF)

#allow flexible page sizes on Android devices
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")

#required to make shared and static libs be compatible on some platforms
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#disable Python (some vcpkg dependencies use it)
set(PythonInterp_FIND_REQUIRED OFF)

#disable app local dependencies as this is classic vpkg, not manifest mode
set(VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "" FORCE)

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

#set up engine project
add_subdirectory(src/Nexus)

if (DEFINED NX_BUILD_UI_TESTS)
    add_subdirectory(src/UITests)
endif()

if (DEFINED NX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/Tests)
endif()

if (DEFINED NX_BUILD_DEMO)
    add_subdirectory(src/Demo)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
endif()

if (DEFINED NX_BUILD_SANDBOX)
    add_subdirectory(src/Sandbox)
endif()

if (DEFINED NX_BUILD_EDITOR)
    add_subdirectory(src/Editor)
endif()