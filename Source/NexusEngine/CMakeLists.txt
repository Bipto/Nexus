cmake_minimum_required(VERSION 3.16)

project ("NexusEngine")

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS Nexus/*.h)
file (GLOB_RECURSE ENGINE_SOURCES 
 Nexus/*.cpp
 Nexus/FileSystem/*.cpp
 Nexus/Graphics/*.cpp
 Nexus/ImGui/*.cpp
 Nexus/Input/*.cpp
 Nexus/Input/*.cpp
 Nexus/Logging/*.cpp
 Nexus/Renderer/*.cpp
 Nexus/Runtime/*.cpp
 Nexus/Runtime/ECS/*.cpp
 
 Platform/*.cpp
 Platform/Vulkan/*.cpp
 Platform/OpenGL/*.cpp
 Platform/D3D11/*.cpp
 Platform/D3D12/*.cpp
 )

add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including dependencies
target_include_directories(NexusEngine PUBLIC 
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
if (WIN32)
  target_include_directories(NexusEngine PUBLIC
    ../../External/tinyfiledialogs/tinyfiledialogs
    ${VULKAN_INCLUDE_DIRS}
    ../../External/VulkanMemoryAllocator/include)
endif()

#add the engines headers to the include path
target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#linking dependencies
target_link_libraries(NexusEngine PUBLIC
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

if(WIN32)
  target_link_libraries(NexusEngine PUBLIC 
    tinyfiledialogs
    Vulkan::Vulkan
    VulkanMemoryAllocator
    )
endif()

if(WIN32 OR ANDROID)
  target_link_libraries(NexusEngine PUBLIC OpenAL)
endif()

#precompiled header
target_precompile_headers(NexusEngine PUBLIC Nexus/nxpch.hpp)

#resources need to be output into a separate folder if building for emscripten
if (NOT EMSCRIPTEN)
    set(path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    cmake_path(GET path PARENT_PATH parentDir)
    set_target_properties(NexusEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${parentDir}/bin)
    file (COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      NexusEngine
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -Os -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -lopenal -sGL_ENABLE_GET_PROC_ADDRESS"
    )

    set_target_properties(NexusEngine PROPERTIES COMPILE_FLAGS "-Os")
endif()