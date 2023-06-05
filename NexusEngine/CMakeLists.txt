cmake_minimum_required(VERSION 3.16)

project ("NexusEngine")
set (CMAKE_CXX_STANDARD 17)

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

set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_SPIRV_TOOLS_INSTALL OFF CACHE BOOL "" FORCE)

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS Core/*.h Runtime/*.h)
file (GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Core/Input/*.cpp Core/Utils/*.cpp Platform/*.cpp Platform/OpenGL*.cpp Platform/DX11/*.cpp Runtime/*.cpp Runtime/ECS/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including OpenGL and Vulkan
target_link_libraries(NexusEngine PUBLIC ${OPENGL_LIBRARIES})
target_include_directories(NexusEngine PUBLIC ${VULKAN_INCLUDE_DIRS})
target_link_libraries(NexusEngine PUBLIC Vulkan::Vulkan)

#including dependencies
target_include_directories(NexusEngine PUBLIC ../External/SDL/include)
target_include_directories(NexusEngine PUBLIC ../External/SDL_mixer/include)
target_include_directories(NexusEngine PRIVATE External/Lua.lua-5.4.4)
target_include_directories(NexusEngine PUBLIC ../External/glm)
target_include_directories(NexusEngine PUBLIC ../External/stb)
target_include_directories(NexusEngine PUBLIC ../External/assimp/include)
target_include_directories(NexusEngine PUBLIC ../External/json/single_include)
target_include_directories(NexusEngine PUBLIC ../External/VulkanMemoryAllocator/include)

if (NOT DEFINED EMSCRIPTEN)
target_include_directories(NexusEngine PUBLIC ../External/tinyfiledialogs/tinyfiledialogs)
target_include_directories(NexusEngine PUBLIC ../External/shaderc/libshaderc)
target_include_directories(NexusEngine PUBLIC ../External/shaderc/SPIRV-Cross/include)
endif()

target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#linking dependencies
target_link_libraries(NexusEngine PUBLIC SDL2main SDL2-static)
target_link_libraries(NexusEngine PUBLIC lua_static)
target_link_libraries(NexusEngine PUBLIC glad)
target_link_libraries(NexusEngine PUBLIC assimp)
target_link_libraries(NexusEngine PUBLIC imgui)
target_link_libraries(NexusEngine PUBLIC imguizmo)
target_link_libraries(NexusEngine PUBLIC VulkanMemoryAllocator)

if(NOT DEFINED EMSCRIPTEN)
target_link_libraries(NexusEngine PUBLIC tinyfiledialogs)
target_link_libraries(NexusEngine PUBLIC shaderc)
target_link_libraries(NexusEngine PUBLIC spirv-cross-cpp spirv-cross-hlsl)
endif()

#precompiled header
target_precompile_headers(NexusEngine PUBLIC Core/nxpch.h)

if (EMSCRIPTEN)
    set_target_properties(
        Editor
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_WEBGL2=1 -sFULL_ES3=1 -lGL --preload-file Resources"
    )

endif()