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

if(WIN32 OR LINUX)
    add_subdirectory(src/Sandbox)
    add_subdirectory(src/Editor)
endif()

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

#external dependencies
add_subdirectory(external/SDL)
add_subdirectory(external/glad)
add_subdirectory(external/glm)
add_subdirectory(external/imgui)
add_subdirectory(external/glslang)
add_subdirectory(external/SPIRV-Cross)
add_subdirectory(external/SPIRV-Headers)
add_subdirectory(external/SPIRV-Tools)
add_subdirectory(external/shaderc)
add_subdirectory(external/libnyquist)
add_subdirectory(external/freetype)
add_subdirectory(external/pocketpy)
add_subdirectory(external/yaml-cpp)

if (WIN32 OR LINUX)
    add_subdirectory(external/VulkanMemoryAllocator)
    set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "external")
endif()

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (WIN32 OR LINUX)
    add_subdirectory(external/tinyfiledialogs)
    add_subdirectory(external/re2)
    add_subdirectory(external/effcee)
    set_target_properties(tinyfiledialogs re2 effcee PROPERTIES FOLDER "external")
endif()

#include libraries to be built dynamically
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_definitions(-DLIBTYPE=SHARED)
add_subdirectory(external/assimp)
if (WIN32 OR ANDROID OR LINUX)
    add_subdirectory(external/OpenAL-Soft)
    set_target_properties(OpenAL PROPERTIES FOLDER "external")
endif()
remove_definitions(-DLIBTYPE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set_target_properties(SDL3-static SDL3_test PROPERTIES FOLDER "external")
set_target_properties(glad PROPERTIES FOLDER "external")
set_target_properties(imgui PROPERTIES FOLDER "external")
set_target_properties(imguizmo PROPERTIES FOLDER "external")
set_target_properties(glslc glslc_exe PROPERTIES FOLDER "external")
set_target_properties(spirv-cross-c spirv-cross-core spirv-cross-cpp
    spirv-cross-glsl spirv-cross-hlsl spirv-cross-msl spirv-cross-reflect
    spirv-tools-pkg-config spirv-tools-shared-pkg-config
    PROPERTIES FOLDER "external")
set_target_properties(shaderc_shared shaderc_static-pkg-config shaderc_util shaderc-pkg-config
    shaderc shaderc_combined_genfile shaderc_combined-pkg-config
    PROPERTIES FOLDER "external")
set_target_properties(libnyquist PROPERTIES FOLDER "external")
set_target_properties(freetype PROPERTIES FOLDER "external")
set_target_properties(pocketpy PROPERTIES FOLDER "external")
set_target_properties(yaml-cpp yaml-cpp-parse yaml-cpp-read yaml-cpp-sandbox PROPERTIES FOLDER "external")
set_target_properties(assimp UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "external")
set_target_properties(add-copyright build_version build-version check-copyright
    common core_tables enum_string_mapping ex-common extinst_tables testdata uninstall zlibstatic
    PROPERTIES FOLDER "external")