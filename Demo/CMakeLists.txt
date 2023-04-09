cmake_minimum_required(VERSION 3.16)

project("Demo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Demo main.cpp)

file (COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders.hlsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/brick.jpg
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})