cmake_minimum_required(VERSION 3.16)

project("Editor")

file (GLOB_RECURSE APP_SOURCES 
main.cpp
UI/*.cpp
UI/Dialogs/*.cpp)

add_executable(Editor main.cpp ${APP_SOURCES})

target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)

target_include_directories(Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})