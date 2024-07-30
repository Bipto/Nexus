cmake_minimum_required(VERSION 3.16)

project ("Nexus")

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS src/Nexus-Core/*.h)
file (GLOB_RECURSE ENGINE_SOURCES 
 src/Nexus-Core/*.cpp
 src/Nexus-Core/FileSystem/*.cpp
 src/Nexus-Core/Graphics/*.cpp
 src/Nexus-Core/ImGui/*.cpp
 src/Nexus-Core/Input/*.cpp
 src/Nexus-Core/Input/*.cpp
 src/Nexus-Core/Logging/*.cpp
 src/Nexus-Core/Renderer/*.cpp
 src/Nexus-Core/Runtime/*.cpp
 src/Nexus-Core/Runtime/ECS/*.cpp
 
 src/Platform/*.cpp
 src/Platform/Vulkan/*.cpp
 src/Platform/OpenGL/*.cpp
 src/Platform/D3D11/*.cpp
 src/Platform/D3D12/*.cpp
 )

add_library(Nexus ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including dependencies
target_include_directories(Nexus PUBLIC 
  ../../External/glm
  ../../External/stb
  ../../External/assimp/include
  ../../External/json/single_include
  ../../External/OpenAL-Soft/include
  ../../External/shaderc/libshaderc
  ../../External/libnyquist/include
  ../../External/SPIRV-Cross/include
  ../../External/SPIRV-Reflect
  ../../External/Freetype/include
  ../../External/pocketpy/include
  ../../External/yaml-cpp/include
  ../../Binaries/dxc_2023_08_14/inc
  )

#dependencies when not building with emscripten
if (WIN32 OR LINUX)
  target_include_directories(Nexus PUBLIC
    ../../External/tinyfiledialogs/tinyfiledialogs
    ${VULKAN_INCLUDE_DIRS}
    ../../External/VulkanMemoryAllocator/include)
endif()

#add the engines headers to the include path
target_include_directories(Nexus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

#linking dependencies
target_link_libraries(Nexus PUBLIC
  ${OPENGL_LIBRARIES}
  SDL3::SDL3
  glad
  assimp
  imgui
  imguizmo
  shaderc
  libnyquist
  spirv-cross-cpp
  spirv-cross-hlsl
  pocketpy
  freetype
  yaml-cpp::yaml-cpp
  )

if(WIN32 OR LINUX)
  target_link_libraries(Nexus PUBLIC 
    tinyfiledialogs
    Vulkan::Vulkan
    VulkanMemoryAllocator
    )
endif()

if(WIN32 OR ANDROID OR LINUX)
  target_link_libraries(Nexus PUBLIC OpenAL)
endif()

#precompiled header
target_precompile_headers(Nexus PUBLIC src/Nexus-Core/nxpch.hpp)

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Nexus
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -Os -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -lopenal -sGL_ENABLE_GET_PROC_ADDRESS"
    )

    set_target_properties(Nexus PROPERTIES COMPILE_FLAGS "-Os")
endif()
