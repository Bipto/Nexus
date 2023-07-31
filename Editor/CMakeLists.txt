cmake_minimum_required(VERSION 3.16)

project("Editor")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file (GLOB_RECURSE APP_HEADERS *.h)
file (GLOB_RECURSE APP_SOURCES *.cpp)
add_executable(Editor ${APP_HEADERS} ${APP_SOURCES})

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    DESTINATION ${CMAKE_BINARY_DIR}/bin)

target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)

if (EMSCRIPTEN)
    set_target_properties(
        Editor
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_WEBGL2=1 -sFULL_ES3=1 -lGL -lopenal --preload-file Resources"
    )
else()
    set(path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    cmake_path(GET path PARENT_PATH parentDir)
    set_target_properties(Editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${parentDir}/bin)
endif()