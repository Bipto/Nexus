cmake_minimum_required(VERSION 3.16)

project("NexusEngine")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

#if we are not building for emscripten, then output all executables into lib and bin folders
#this is needed to ensure that dynamic libraries (.dll) are in the same folder as the executables
if (NOT EMSCRIPTEN)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
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
    add_compile_definitions(NX_PLATFORM_WINDOWS)
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
add_subdirectory(Source/BuildConfig)
add_subdirectory(Source/NexusEngine)
add_subdirectory(Source/Demo)

if(WIN32 OR LINUX)
    add_subdirectory(Source/Test)
    add_subdirectory(Source/Editor)
endif()

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

#external dependencies
add_subdirectory(External/SDL)
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
add_subdirectory(External/freetype)
add_subdirectory(External/pocketpy)
add_subdirectory(External/yaml-cpp)

if (WIN32 OR LINUX)
    add_subdirectory(External/VulkanMemoryAllocator)
endif()

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (WIN32 OR LINUX)
    add_subdirectory(External/tinyfiledialogs)
    add_subdirectory(External/re2)
    add_subdirectory(External/effcee)
endif()

if (WIN32 OR ANDROID OR LINUX)
    add_definitions(-DLIBTYPE=SHARED)
    add_subdirectory(External/OpenAL-Soft)
    remove_definitions(-DLIBTYPE)
endif()

if (WIN32)
    file(COPY Binaries/dxc_2023_08_14/bin/x64/dxcompiler.dll 
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY Binaries/dxc_2023_08_14/bin/x64/dxil.dll 
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
