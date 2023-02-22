cmake_minimum_required(VERSION 3.16)

project("Editor")

file (GLOB_RECURSE APP_HEADERS *.h)
file (GLOB_RECURSE APP_SOURCES *.cpp)
add_executable(Editor ${APP_HEADERS} ${APP_SOURCES})

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/brick.jpg
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/wall.jpg
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders/Basic.shader
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders)