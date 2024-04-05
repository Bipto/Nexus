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

set(path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
cmake_path(GET path PARENT_PATH parentDir)
set_target_properties(Editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${parentDir}/bin)
file (COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})