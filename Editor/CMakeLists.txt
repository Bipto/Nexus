cmake_minimum_required(VERSION 3.16)

project("Editor")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file (GLOB_RECURSE APP_HEADERS *.h)
file (GLOB_RECURSE APP_SOURCES *.cpp)
add_executable(Editor ${APP_HEADERS} ${APP_SOURCES})

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Textures/brick.jpg
DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/Textures)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Textures/wall.jpg
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/Textures)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders/GLSL.shader
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders/GLSLES.shader
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/test.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources)