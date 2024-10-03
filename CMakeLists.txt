cmake_minimum_required(VERSION 3.16)

project("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

#if we are not building for emscripten, then output all executables into lib and bin folders
#this is needed to ensure that dynamic libraries (.dll) are in the same folder as the executables
if (NOT EMSCRIPTEN)
    find_package(Vulkan REQUIRED)
endif()

if (WIN32 OR EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
endif()

#platform specific configurations
if (WIN32)
    add_compile_definitions(NX_PLATFORM_WINDOWS)
    add_compile_definitions(NX_PLATFORM_D3D11)
    add_compile_definitions(NX_PLATFORM_D3D12)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_VULKAN)
    add_compile_definitions(NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_SDL_NET)
    add_compile_definitions(NX_PLATFORM_GL_MULTISAMPLE_SWAPCHAIN)
    add_compile_definitions(NX_PLATFORM_SUPPORTS_MULTI_WINDOW)
    add_compile_definitions(NX_PLATFORM_GL_DESKTOP)
elseif(LINUX)
    add_compile_definitions(NX_PLATFORM_VULKAN)
    add_compile_definitions(NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_SDL_NET)
    add_compile_definitions(NX_PLATFORM_SUPPORTS_MULTI_WINDOW)
elseif (EMSCRIPTEN)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_WEBGL2)
    add_compile_definitions(NX_PLATFORM_GL_MULTISAMPLE_SWAPCHAIN)
elseif (ANDROID)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_OPENAL)
endif()

#set up engine projects
add_subdirectory(src/BuildConfig)
add_subdirectory(src/Nexus)
add_subdirectory(src/Demo)

enable_testing()
add_subdirectory(src/Tests)

if(WIN32 OR LINUX)
    add_subdirectory(src/Sandbox)
endif()