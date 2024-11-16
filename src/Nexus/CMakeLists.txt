cmake_minimum_required(VERSION 3.16)

project ("Nexus")

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

#external dependencies
add_subdirectory(external/SDL)
add_subdirectory(external/glad)

if (WIN32)
  add_subdirectory(external/glad_wgl)
elseif(LINUX)
  add_subdirectory(external/glad_glx)
elseif(ANDROID OR __ANDROID__)
  add_subdirectory(external/glad_egl)
endif()

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
    set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20
      CXX_EXTENSIONS OFF)
endif()

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (WIN32 OR LINUX)
    add_subdirectory(external/tinyfiledialogs)
    set_target_properties(tinyfiledialogs PROPERTIES FOLDER "external")
endif()

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS src/Nexus-Core/*.h)
file (GLOB_RECURSE ENGINE_SOURCES 
 src/Nexus-Core/*.hpp
 src/Nexus-Core/*.cpp
 src/Nexus-Core/FileSystem/*.hpp
 src/Nexus-Core/FileSystem/*.cpp
 src/Nexus-Core/Graphics/*.hpp
 src/Nexus-Core/Graphics/*.cpp
 src/Nexus-Core/ImGui/*.hpp
 src/Nexus-Core/ImGui/*.cpp
 src/Nexus-Core/Input/*.hpp
 src/Nexus-Core/Input/*.cpp
 src/Nexus-Core/Logging/*.hpp
 src/Nexus-Core/Logging/*.cpp
 src/Nexus-Core/Renderer/*.hpp
 src/Nexus-Core/Renderer/*.cpp
 src/Nexus-Core/Runtime/*.hpp
 src/Nexus-Core/Runtime/*.cpp
 src/Nexus-Core/Runtime/ECS/*.hpp
 src/Nexus-Core/Runtime/ECS/*.cpp
 src/Nexus-Core/Layers/*.hpp
 src/Nexus-Core/Layers/*.cpp
 
 src/Platform/*.hpp
 src/Platform/*.cpp
 src/Platform/Vulkan/*.hpp
 src/Platform/Vulkan/*.cpp
 src/Platform/OpenGL/*.hpp
 src/Platform/OpenGL/*.cpp
 src/Platform/OpenGL/WGL/*.hpp
 src/Platform/OpenGL/WGL/*.cpp
 src/Platform/OpenGL/WebGL/*.hpp
 src/Platform/OpenGL/WebGL/*.cpp
 src/Platform/D3D11/*.hpp
 src/Platform/D3D11/*.cpp
 src/Platform/D3D12/*.hpp
 src/Platform/D3D12/*.cpp
 src/Platform/SDL3/*.hpp
 src/Platform/SDL3/*.cpp
 src/Platform/Software/*.hpp
 src/Platform/Software/*.cpp
 )

add_library(Nexus ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including dependencies
target_include_directories(Nexus PUBLIC 
  external/glm
  external/stb
  external/assimp/include
  external/json/single_include
  external/OpenAL-Soft/include
  external/shaderc/libshaderc
  external/libnyquist/include
  external/SPIRV-Cross/include
  external/SPIRV-Reflect
  external/Freetype/include
  external/pocketpy/include
  external/yaml-cpp/include
  binaries/dxc_2023_08_14/inc
  )

#dependencies when not building with emscripten
if (WIN32 OR LINUX)
  target_include_directories(Nexus PUBLIC
    external/tinyfiledialogs/tinyfiledialogs
    ${VULKAN_INCLUDE_DIRS}
    external/VulkanMemoryAllocator/include)
endif()

#add the engines headers to the include path
target_include_directories(Nexus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

#precompiled header
target_precompile_headers(Nexus PUBLIC src/Nexus-Core/nxpch.hpp)

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Nexus
        PROPERTIES
        LINK_FLAGS
        "--std=c++20 -O3 -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS"
    )

    set_target_properties(Nexus PROPERTIES COMPILE_FLAGS "-Os")
endif()

#include libraries to be built dynamically
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_definitions(-DLIBTYPE=SHARED)
add_subdirectory(external/assimp)
if (WIN32 OR ANDROID OR LINUX)
    add_subdirectory(external/OpenAL-Soft)
    set_target_properties(OpenAL PROPERTIES FOLDER "src/Nexus/external")
endif()
remove_definitions(-DLIBTYPE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set_target_properties(SDL3-static SDL3_test PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(glad PROPERTIES FOLDER "src/Nexus/external")

if (WIN32)
  set_target_properties(glad_wgl PROPERTIES FOLDER "src/Nexus/external")
endif()

set_target_properties(imgui PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(imguizmo PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(glslc glslc_exe PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(spirv-cross-c spirv-cross-core spirv-cross-cpp
    spirv-cross-glsl spirv-cross-hlsl spirv-cross-msl spirv-cross-reflect
    spirv-tools-pkg-config spirv-tools-shared-pkg-config
    PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(shaderc_shared shaderc_static-pkg-config shaderc_util shaderc-pkg-config
    shaderc shaderc_combined_genfile shaderc_combined-pkg-config
    PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(libnyquist PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(freetype PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(pocketpy PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(yaml-cpp yaml-cpp-parse yaml-cpp-read yaml-cpp-sandbox PROPERTIES FOLDER "src/Nexus/external")
set_target_properties(assimp PROPERTIES FOLDER "src/Nexus/external")

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

if (WIN32)
  target_link_libraries(Nexus PUBLIC glad_wgl)
endif()

if (ANDROID OR __ANDROID__)
  target_link_libraries(Nexus PUBLIC glad_egl)
endif()

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

macro (nexus_copy_resources)
  #copy engine resources to output
  get_target_property(Nexus_SOURCE_DIR Nexus SOURCE_DIR)

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${Nexus_SOURCE_DIR}/resources/engine
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/engine)

  message($<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/engine)
endmacro()

macro(nexus_copy_required_binaries)
  if (WIN32)
    get_target_property(Nexus_SOURCE_DIR Nexus SOURCE_DIR)

    add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
          ${Nexus_SOURCE_DIR}/binaries/dxc_2023_08_14/bin/x64/dxcompiler.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )

  add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
          ${Nexus_SOURCE_DIR}/binaries/dxc_2023_08_14/bin/x64/dxil.dll
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
  endif()
endmacro()