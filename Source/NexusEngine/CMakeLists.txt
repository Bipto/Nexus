cmake_minimum_required(VERSION 3.16)

project ("NexusEngine")
set (CMAKE_CXX_STANDARD 17)

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS Core/*.h Runtime/*.h)
file (GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Core/Input/*.cpp Core/Utils/*.cpp Platform/*.cpp Platform/OpenGL*.cpp Platform/DX11/*.cpp Runtime/*.cpp Runtime/ECS/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including dependencies
target_include_directories(NexusEngine PUBLIC 
  ../../External/SDL/include
  ../../External/Lua.lua-5.4.4
  ../../External/glm
  ../../External/stb
  ../../External/assimp/include
  ../../External/json/single_include
  ../../External/OpenAL-Soft/include
  ../../External/shaderc/libshaderc
  ../../External/libnyquist/include
  ../../External/shaderc/SPIRV-Cross/include
  ${VULKAN_INCLUDE_DIRS}
  ../../External/VulkanMemoryAllocator/include)

#dependencies when not building with emscripten
if (NOT DEFINED EMSCRIPTEN)
  target_include_directories(NexusEngine PUBLIC
    ../../External/tinyfiledialogs/tinyfiledialogs
    ../../External/SDL_net)
endif()

#add the engines headers to the include path
target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#linking dependencies
target_link_libraries(NexusEngine PUBLIC
  ${OPENGL_LIBRARIES} 
  SDL2main
  SDL2-static
  lua_static
  glad
  assimp
  imgui
  imguizmo
  shaderc
  libnyquist
  spirv-cross-cpp
  spirv-cross-hlsl
  Vulkan::Vulkan
  VulkanMemoryAllocator)

if(NOT DEFINED EMSCRIPTEN)
  target_link_libraries(NexusEngine PUBLIC 
    tinyfiledialogs
    OpenAL
    SDL2_net)
endif()

#precompiled header
target_precompile_headers(NexusEngine PUBLIC Core/nxpch.hpp)