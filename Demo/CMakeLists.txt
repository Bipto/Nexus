cmake_minimum_required(VERSION 3.16)

project("Demo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Demo main.cpp)

file (COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/shader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_shader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/brick.jpg
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (EMSCRIPTEN)
    set_target_properties(
        Demo
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_WEBGL2=1 -sFULL_ES3=1 -lGL --preload-file brick.jpg"
    )
    add_compile_options("lGL")
    string(APPEND CMAKE_EXE_LINKER_FLAGS "-lGL")

endif()