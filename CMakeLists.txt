cmake_minimum_required(VERSION 3.16)

project("NexusEngine")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SHADERC_SKIP_TESTS OFF CACHE BOOL "" FORCE)

#google test - required for shaderc
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#if we are not building for emscripten, then output all executables into lib and bin folders
#this is needed to ensure that dynamic libraries (.dll) are in the same folder as the executables
if (WIN32)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    find_package(Vulkan REQUIRED)
endif()

#platform specific configurations
if (WIN32)
    add_compile_definitions(NX_PLATFORM_WINDOWS)
    add_compile_definitions(NX_PLATFORM_DX11)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_VULKAN)
    add_compile_definitions(NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_SDL_NET)
elseif (EMSCRIPTEN)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_OPENAL)
endif()

if (WIN32 OR EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
endif()

#set up engine projects
add_subdirectory(Source/Demo)
add_subdirectory(Source/NexusEngine)

if(WIN32)
    add_subdirectory(Source/Editor)
    add_subdirectory(Source/NetworkingTest/Client)
    add_subdirectory(Source/NetworkingTest/Server)
    add_subdirectory(Source/Test)
    add_subdirectory(Source/ShaderGen)
endif()

add_subdirectory(Source/BuildConfig)

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

if (WIN32)
    add_subdirectory(External/VulkanMemoryAllocator)
endif()

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (WIN32)
    add_subdirectory(External/tinyfiledialogs)
    add_subdirectory(External/re2)
    add_subdirectory(External/effcee)
    add_subdirectory(External/SDL_net)
    add_definitions(-DLIBTYPE=SHARED)
    add_subdirectory(External/OpenAL-Soft)
    remove_definitions(-DLIBTYPE)
endif()