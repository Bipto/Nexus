cmake_minimum_required(VERSION 3.16)

project("Editor")

add_executable(Editor main.cpp)
target_include_directories(Editor PUBLIC ../Nexus/src)
target_link_libraries(Editor PUBLIC Nexus)