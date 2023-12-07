cmake_minimum_required(VERSION 3.16)

project ("NexusEngine")

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS Nexus/*.h)
file (GLOB_RECURSE ENGINE_SOURCES 
 Nexus/*.cpp
 Nexus/Input/*.cpp
 Nexus/FileSystem/*.cpp
 Platform/*.cpp
 Platform/OpenGL*.cpp
 Platform/D3D11/*.cpp
 Platform/D3D12/*.cpp
 Runtime/*.cpp
 Runtime/ECS/*.cpp)

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
  ../../External/SPIRV-Cross/include
  ../../External/SPIRV-Reflect
  ../../External/include
  )

#dependencies when not building with emscripten
if (WIN32)
  target_include_directories(NexusEngine PUBLIC
    ../../External/tinyfiledialogs/tinyfiledialogs
    ../../External/SDL_net
    ${VULKAN_INCLUDE_DIRS}
    ../../External/VulkanMemoryAllocator/include)
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
  freetype
  )

if(WIN32)
  target_link_libraries(NexusEngine PUBLIC 
    tinyfiledialogs
    SDL2_net
    Vulkan::Vulkan
    VulkanMemoryAllocator)
endif()

if(WIN32 OR ANDROID)
  target_link_libraries(NexusEngine PUBLIC OpenAL)
endif()

#precompiled header
target_precompile_headers(NexusEngine PUBLIC Nexus/nxpch.hpp)
#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      NexusEngine
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -Os -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -fsanitize=address -lopenal --preload-file Resources"
    )

    set_target_properties(NexusEngine PROPERTIES COMPILE_FLAGS "-Os")
endif()

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})