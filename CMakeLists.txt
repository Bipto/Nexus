cmake_minimum_required(VERSION 3.30)

project("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

set(NX_ROOT_BUILD OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(NX_ROOT_BUILD ON)
endif()

#set up engine project
add_subdirectory(src/Nexus)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if (DEFINED NX_BUILD_DEMO)
        add_subdirectory(src/Demo)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)
    endif()

    if (DEFINED NX_BUILD_EDITOR)
        add_subdirectory(src/Editor)
    endif()

    if (DEFINED NX_BUILD_TESTS)
        enable_testing()
        add_subdirectory(src/Tests)
    endif()
endif()