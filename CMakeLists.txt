cmake_minimum_required(VERSION 3.16)

project("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

#set up engine projects
add_subdirectory(src/BuildConfig)
add_subdirectory(src/Nexus)
add_subdirectory(src/Demo)

enable_testing()
add_subdirectory(src/Tests)

if(WIN32 OR LINUX)
    add_subdirectory(src/Sandbox)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)