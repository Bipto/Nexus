cmake_minimum_required(VERSION 3.16)

project("Test")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Test WIN32 main.cpp)

target_include_directories(Test PUBLIC NexusEngine)
target_link_libraries(Test PUBLIC NexusEngine)

set(path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
cmake_path(GET path PARENT_PATH parentDir)
set_target_properties(Demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${parentDir}/bin)
file (COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})