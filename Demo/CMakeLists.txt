cmake_minimum_required(VERSION 3.16)

project("Demo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Demo main.cpp)

target_include_directories(Demo PUBLIC NexusEngine)
target_link_libraries(Demo PUBLIC NexusEngine)

if (EMSCRIPTEN)
file (COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
file (COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()

if (EMSCRIPTEN)
    set_target_properties(
        Demo
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -lGL -lopenal --preload-file Resources"
    )
endif()