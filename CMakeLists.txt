cmake_minimum_required(VERSION 3.16)

project("NexusEngine")
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(Editor)
add_subdirectory(Demo)
add_subdirectory(NexusEngine)

add_subdirectory(External/SDL)
add_subdirectory(External/Lua)
add_subdirectory(External/glad)
add_subdirectory(External/glm)
add_subdirectory(External/imgui)
add_subdirectory(External/tinyfiledialogs)

set (BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(External/assimp)

#Editor
target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)

target_include_directories(Editor PUBLIC External/tinyfiledialogs/tinyfiledialogs)
target_link_libraries(Editor PUBLIC tinyfiledialogs)

target_include_directories(Demo PUBLIC NexusEngine)
target_link_libraries(Demo PUBLIC NexusEngine)

if (EMSCRIPTEN)
    set_target_properties(
        Editor
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -s ALLOW_MEMORY_GROWTH=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 -s FULL_ES3=1 --preload-file Resources"
    )
endif()