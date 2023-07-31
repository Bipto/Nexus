cmake_minimum_required(VERSION 3.16)

project("NexusEngine")
set (CMAKE_CXX_STANDARD 17)

#if we are not building for emscripten, then output all executables into lib and bin folders
#this is needed to ensure that dynamic libraries (.dll) are in the same folder as the executables
if (NOT EMSCRIPTEN)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

#platform specific configurations
if (WIN32)
    add_compile_definitions(NX_PLATFORM_WINDOWS)
    add_compile_definitions(NX_PLATFORM_DX11)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_SDL_NET)
endif()

if (EMSCRIPTEN)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_OPENAL)

    set(CMAKE_CXX_FLAGS_DEBUG "-O3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

#set up engine projects
add_subdirectory(Demo)
add_subdirectory(NexusEngine)

if(NOT DEFINED EMSCRIPTEN)
    add_subdirectory(Editor)
    add_subdirectory(NetworkingTest/Client)
    add_subdirectory(NetworkingTest/Server)
endif()

add_subdirectory(BuildConfig)

#external dependencies
add_subdirectory(External/SDL)
add_subdirectory(External/Lua)
add_subdirectory(External/glad)
add_subdirectory(External/glm)
add_subdirectory(External/imgui)
add_subdirectory(External/assimp)
add_subdirectory(External/glslang)
add_subdirectory(External/SPIRV-Cross)
add_subdirectory(External/SPIRV-Headers)
add_subdirectory(External/SPIRV-Tools)
add_subdirectory(External/shaderc)
add_subdirectory(External/libnyquist)

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (NOT DEFINED EMSCRIPTEN)
    add_subdirectory(External/tinyfiledialogs)
    add_subdirectory(External/re2)
    add_subdirectory(External/effcee)
    add_subdirectory(External/SDL_net)

    add_definitions(-DLIBTYPE=SHARED)
    add_subdirectory(External/OpenAL-Soft)
    remove_definitions(-DLIBTYPE)
endif()