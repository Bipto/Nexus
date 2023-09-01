cmake_minimum_required(VERSION 3.16)

project("Demo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Demo WIN32 main.cpp)

target_include_directories(Demo PUBLIC NexusEngine)
target_link_libraries(Demo PUBLIC NexusEngine)

#resources need to be output into a separate folder if building for emscripten
if (EMSCRIPTEN)
file (COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
    set(path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    cmake_path(GET path PARENT_PATH parentDir)
    set_target_properties(Demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${parentDir}/bin)
    file (COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
        Demo
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -Os -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -fsanitize=address -lopenal --preload-file Resources"
    )

    set_target_properties(Demo PROPERTIES COMPILE_FLAGS "-Os")
endif()