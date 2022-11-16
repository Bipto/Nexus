cmake_minimum_required(VERSION 3.16)

project("NexusEngine")

find_package(Python REQUIRED COMPONENTS Interpreter Development)

file (GLOB_RECURSE ENGINE_HEADERS NexusEngine/*.h)
file (GLOB_RECURSE ENGINE_SOURCES NexusEngine/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

file (GLOB_RECURSE APP_HEADERS Application/*.h)
file (GLOB_RECURSE APP_SOURCES Application/*.cpp)
add_executable(Editor ${APP_HEADERS} ${APP_SOURCES})

find_package(OpenGL REQUIRED)
target_link_libraries(NexusEngine PUBLIC ${OPENGL_LIBRARIES})

add_subdirectory(External/SDL)
#add_subdirectory(External/Lua)
add_subdirectory(External/glad)

#NexusEngine
target_include_directories(NexusEngine PUBLIC External/SDL/include)
target_include_directories(NexusEngine PUBLIC External/SDL_mixer/include)
#target_include_directories(NexusEngine PUBLIC External/Lua.lua-5.4.4)
target_include_directories(NexusEngine PUBLIC ${Python_INCLUDE_DIRS})

target_link_libraries(NexusEngine PUBLIC SDL2main SDL2-static)
target_link_directories(NexusEngine PUBLIC ${Python_LIBRARY_DIRS})
target_link_libraries(NexusEngine PUBLIC ${Python_LIBRARIES})
target_link_libraries(NexusEngine PUBLIC glad)
#target_link_libraries(NexusEngine PUBLIC lua_static)

#Editor
target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)

if (EMSCRIPTEN)
    set_target_properties(
        Editor
        PROPERTIES
        LINK_FLAGS
        "-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 -s FULL_ES3=1"
    )
endif()

target_precompile_headers(NexusEngine PUBLIC NexusEngine/Core/nxpch.h)