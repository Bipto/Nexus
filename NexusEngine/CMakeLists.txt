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

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS Core/*.h Runtime/*.h)
file (GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Core/Input/*.cpp Core/Utils/*.cpp Platform/*.cpp Platform/OpenGL*.cpp Platform/DX11/*.cpp Runtime/*.cpp Runtime/ECS/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including dependencies
target_include_directories(NexusEngine PUBLIC 
  ../External/SDL/include
  ../External/Lua.lua-5.4.4
  ../External/glm
  ../External/stb
  ../External/assimp/include
  ../External/json/single_include
  ../External/OpenAL-Soft/include)

#dependencies when not building with emscripten
if (NOT DEFINED EMSCRIPTEN)
  target_include_directories(NexusEngine PUBLIC
    ../External/tinyfiledialogs/tinyfiledialogs
    ../External/shaderc/libshaderc
    ../External/shaderc/SPIRV-Cross/include
    ../External/libnyquist/include
    ../External/SDL_net)
endif()

#add the engines headers to the include path
target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#linking dependencies
target_link_libraries(NexusEngine PUBLIC 
  SDL2main
  SDL2-static
  lua_static
  glad
  assimp
  imgui
  imguizmo)

if(NOT DEFINED EMSCRIPTEN)
  target_link_libraries(NexusEngine PUBLIC 
    tinyfiledialogs
    shaderc
    spirv-cross-cpp
    spirv-cross-hlsl
    OpenAL
    libnyquist
    SDL2_net)
endif()

#including OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(NexusEngine PUBLIC ${OPENGL_LIBRARIES})

#precompiled header
target_precompile_headers(NexusEngine PUBLIC Core/nxpch.h)