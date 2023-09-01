cmake_minimum_required(VERSION 3.16)

project("ShaderGen")

add_executable(ShaderGen main.cpp)

target_include_directories(ShaderGen PUBLIC NexusEngine)
target_link_libraries(ShaderGen PUBLIC NexusEngine)