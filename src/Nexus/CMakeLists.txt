cmake_minimum_required(VERSION 3.28)

project ("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

include(FetchContent)

if (NOT DEFINED NX_BUILD_TYPE OR EMSCRIPTEN)
  set(NX_BUILD_TYPE STATIC)
endif()

if (NX_BUILD_TYPE STREQUAL "SHARED")
  add_compile_definitions(NX_BUILD_DLL=1)
  add_compile_definitions(NX_EXPORT_API=1)
endif()

add_subdirectory(cmake)

set(NEXUS_INCLUDE_DIRS "external/renderdoc")
set(NEXUS_LINKED_LIBRARIES "")
set(NEXUS_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_COMPILE_DEFINITIONS "")

set(SHADERC_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
set(LIBNYQUIST_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_UNZIP OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(RE2_BUILD_FRAMEWORK OFF CACHE BOOL "" FORCE)
set(RE2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_SPIRV_TOOLS_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/imgui)
list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad2)

include (FetchContent)

#platform specific configurations
if (WIN32)
  list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WINDOWS=1 NOMINMAX=1 WIN32_LEAN_AND_MEAN=1)
  set(NX_PLATFORM_WGL ON CACHE BOOL "")
elseif(LINUX)
  list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_LINUX=1)
  set(NX_PLATFORM_EGL ON CACHE BOOL "")
elseif(EMSCRIPTEN)
  list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WEB=1)
elseif(ANDROID)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_ANDROID=1)
  set(NX_PLATFORM_EGL ON CACHE BOOL "")
endif()

if (DEFINED NX_PLATFORM_OPENGL)
  if (NOT ANDROID)
    find_package(OpenGL REQUIRED)
    list (APPEND NEXUS_LINKED_LIBRARIES ${OPENGL_LIBRARIES})
  endif()
  
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_OPENGL=1)

  if (WIN32)
      list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WGL=1)
      list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_GL_DESKTOP=1)

  elseif(LINUX OR ANDROID OR __ANDROID__)
    if (LINUX)
      list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_GL_DESKTOP=1)
      find_package(PkgConfig REQUIRED)
      pkg_check_modules(EGL REQUIRED egl)
    endif()

    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_OPENGLES)
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_EGL=1)
    list (APPEND NEXUS_INCLUDE_DIRS ${EGL_INCLUDE_DIRS})
    list(APPEND NEXUS_LINKED_LIBRARIES ${EGL_LIBRARIES})

    if (ANDROID)
      find_library(GLES3_LIB GLESv3)
      list (APPEND NEXUS_LINKED_LIBARIES EGL GLESv2 GLESv3)
    endif()

  elseif (EMSCRIPTEN)
  
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WEBGL=1)
  endif()

endif()

if (DEFINED NX_PLATFORM_D3D12)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_D3D12=1)

  FetchContent_Declare(
    D3D12MemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
    GIT_TAG 330170f
  )
  FetchContent_MakeAvailable(D3D12MemoryAllocator)
  list (APPEND NEXUS_LINKED_LIBRARIES D3D12MemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS external/winpixeventruntime.1.0.240308001/include)
endif()

if (DEFINED NX_PLATFORM_VULKAN)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_VULKAN=1)
  find_package(Vulkan REQUIRED)

  FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG 1076b34
  )
  FetchContent_MakeAvailable(VulkanMemoryAllocator)


  #list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/VulkanMemoryAllocator)
  list (APPEND NEXUS_LINKED_LIBRARIES Vulkan::Vulkan VulkanMemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${VULKAN_INCLUDE_DIRS})
endif()

if (DEFINED NX_PLATFORM_OPENAL)
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_OPENAL=1)

    if (EMSCRIPTEN)
      find_package(OpenAL REQUIRED)
      list (APPEND NEXUS_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
      list (APPEND NEXUS_LINKED_LIBARIES ${OPENAL_LIBRARY})
    else()
      FetchContent_Declare(
          OpenAL-Soft
          GIT_REPOSITORY https://github.com/kcat/openal-soft.git
          GIT_TAG ea84e1d
      )
      FetchContent_MakeAvailable(OpenAL-Soft)
      list (APPEND NEXUS_LINKED_LIBRARIES OpenAL)
    endif()
    
endif()

#external dependencies

set(ENGINE_SOURCES "include/Nexus.hpp" "src/Nexus.cpp")
file (GLOB_RECURSE NEXUS_SOURCES "include/*.hpp" "src/Nexus-Core/*.cpp")
list (APPEND ENGINE_SOURCES ${NEXUS_SOURCES})

if (DEFINED NX_PLATFORM_D3D12)
  file (GLOB_RECURSE NEXUS_D3D12_SOURCES "src/Platform/D3D12/*.cpp" "src/Platform/D3D12/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_D3D12_SOURCES})
endif()

if (DEFINED NX_PLATFORM_OPENAL)
  file (GLOB_RECURSE NEXUS_OPENAL_SOURCES "src/Platform/OpenAL/*.cpp" "src/Platform/OpenAL/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_OPENAL_SOURCES})
endif()

if (DEFINED NX_PLATFORM_OPENGL)
  file (GLOB NEXUS_OPENGL_SOURCES "src/Platform/OpenGL/*.cpp" "src/Platform/OpenGL/*.hpp")

  if (EMSCRIPTEN)
    file (GLOB_RECURSE NEXUS_OPENGL_WEBGL_SOURCES "src/Platform/OpenGL/Backends/WebGL/*.cpp" "src/Platform/OpenGL/Backends/WebGL/*.hpp")
    list (APPEND ENGINE_SOURCES ${NEXUS_OPENGL_WEBGL_SOURCES})
  else()
    file (GLOB_RECURSE NEXUS_OPENGL_DEFAULT_SOURCES "src/Platform/OpenGL/Backends/Default/*.cpp" "src/Platform/OpenGL/Backends/Default/*.hpp")
    list (APPEND ENGINE_SOURCES ${NEXUS_OPENGL_DEFAULT_SOURCES})
  endif()

  list (APPEND ENGINE_SOURCES ${NEXUS_OPENGL_SOURCES})
endif()

file (GLOB NEXUS_OPENGL_CONTEXT_HEADERS "src/Platform/OpenGL/Context/*.hpp")
list (APPEND ENGINE_SOURCES ${NEXUS_OPENGL_CONTEXT_HEADERS})

if (DEFINED NX_PLATFORM_EGL)
  file (GLOB_RECURSE NEXUS_EGL_SOURCES "src/Platform/OpenGL/Context/EGL/*.cpp" "src/Platform/OpenGL/Context/EGL/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_EGL_SOURCES})
endif()

if (DEFINED NX_PLATFORM_WEBGL)
  file (GLOB_RECURSE NEXUS_WEBGL_SOURCES "src/Platform/OpenGL/Context/WebGL/*.cpp" "src/Platform/OpenGL/Context/WebGL/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_WEBGL_SOURCES})
endif()

if (DEFINED NX_PLATFORM_WGL)
    file (GLOB_RECURSE NEXUS_WGL_SOURCES "src/Platform/OpenGL/Context/WGL/*.cpp" "src/Platform/OpenGL/Context/WGL/*.hpp")
    list (APPEND ENGINE_SOURCES ${NEXUS_WGL_SOURCES})
endif()

file (GLOB_RECURSE NEXUS_SDL_SOURCES "src/Platform/SDL3/*.cpp" "src/Platform/SDL3/*.hpp")
list (APPEND ENGINE_SOURCES ${NEXUS_SDL_SOURCES})

if (DEFINED NX_PLATFORM_VULKAN)
    file (GLOB NEXUS_VULKAN_SOURCES "src/Platform/Vulkan/*.cpp" "src/Platform/Vulkan/*.hpp")
    list (APPEND ENGINE_SOURCES ${NEXUS_VULKAN_SOURCES})

    if (WIN32)
      list (APPEND ENGINE_SOURCES src/Platform/Vulkan/Windows/PlatformVkWindows.cpp)
      list (APPEND NEXUS_COMPILE_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR=1)
    elseif (LINUX)
      list (APPEND ENGINE_SOURCES src/Platform/Vulkan/Xlib/PlatformVkXlib.cpp)
      list (APPEND NEXUS_COMPILE_DEFINITIONS VK_USE_PLATFORM_XLIB_KHR=1)
    elseif(ANDROID)
      list (APPEND ENGINE_SOURCES src/Platform/Vulkan/Android/PlatformVkAndroid.cpp)
      list (APPEND NEXUS_COMPILE_DEFINITIONS VK_USE_PLATFORM_ANDROID_KHR=1)
    endif()
endif()

if (WIN32)
  file (GLOB_RECURSE NEXUS_WINDOWS_SOURCES "src/Platform/Windows/*cpp" "include/Platform/Windows/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_WINDOWS_SOURCES})
endif()

if (LINUX OR EMSCRIPTEN OR ANDROID)
  file (GLOB_RECURSE NEXUS_X11_SOURCES "src/Platform/Unix/*.cpp" "include/Platform/Unix/*.hpp")
  list (APPEND ENGINE_SOURCES  ${NEXUS_X11_SOURCES})
endif()

add_library(Nexus ${NX_BUILD_TYPE} ${ENGINE_SOURCES})
nexus_setup_filters(${ENGINE_SOURCES})

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG dee2414
)
FetchContent_MakeAvailable(SDL3)
target_link_libraries(Nexus PUBLIC SDL3::SDL3)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 2f86d13
)
FetchContent_MakeAvailable(yaml-cpp)
target_link_libraries(Nexus PUBLIC yaml-cpp::yaml-cpp)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG a38ec50
)
FetchContent_MakeAvailable(assimp)
target_link_libraries(Nexus PRIVATE assimp)

FetchContent_Declare(
  libnyquist
  GIT_REPOSITORY https://github.com/ddiakopoulos/libnyquist.git
  GIT_TAG 767efd9
)
FetchContent_MakeAvailable(libnyquist)
target_link_libraries(Nexus PRIVATE libnyquist)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 2d4c4b4
)
FetchContent_MakeAvailable(glm)
target_link_libraries(Nexus PUBLIC glm::glm)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG ca0247a
)
FetchContent_MakeAvailable(freetype)
target_link_libraries(Nexus PRIVATE freetype)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG f58f558
)
FetchContent_MakeAvailable(stb)
target_include_directories(Nexus PUBLIC ${stb_SOURCE_DIR})

FetchContent_Declare(
  spirv-cross
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
  GIT_TAG 0a88b2d
)
FetchContent_MakeAvailable(spirv-cross)

FetchContent_Declare(
  spirv-headers
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
  GIT_TAG a863779
)
FetchContent_MakeAvailable(spirv-headers)

FetchContent_Declare(
  spirv-tools
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
  GIT_TAG aa6256a
)
FetchContent_MakeAvailable(spirv-tools)

FetchContent_Declare(
  glslang
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG fcf4e92
)
FetchContent_MakeAvailable(glslang)

FetchContent_Declare(
  shaderc
  GIT_REPOSITORY https://github.com/Bipto/shaderc.git
  GIT_TAG de65558
)
FetchContent_MakeAvailable(shaderc)

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list (APPEND NEXUS_LINKED_LIBRARIES  
  spirv-cross-cpp
  spirv-cross-hlsl
  glslang
  shaderc
  imgui
  imguizmo
  glad2)

#precompiled header
#target_precompile_headers(Nexus PRIVATE include/Nexus-Core/nxpch.hpp src/Nexus-Core/nxpch.cpp)

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Nexus
        PROPERTIES
        LINK_FLAGS
        "--std=c++20 -O3 -flto -sALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS"
    )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lidbfs.js")
    target_link_options(Nexus PRIVATE -lidbfs.js)
endif()

nexus_add_all_subdirs("${NEXUS_LIBRARY_SUBDIRECTORIES}")

if (DEFINED NX_PLATFORM_VULKAN)
  set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD 17)
endif()

set(NX_INCLUDE_VISIBILITY PUBLIC)
set(NX_LINK_VISIBILITY PUBLIC)
set(NX_COMPILE_DEFINITION_VISIBILITY PUBLIC)

target_include_directories(Nexus ${NX_INCLUDE_VISIBILITY} SYSTEM ${NEXUS_INCLUDE_DIRS})
target_link_libraries(Nexus ${NX_LINK_VISIBILITY} ${NEXUS_LINKED_LIBRARIES})
target_compile_definitions(Nexus ${NX_COMPILE_DEFINITION_VISIBILITY} ${NEXUS_COMPILE_DEFINITIONS})
nexus_structure_folders()
nexus_setup_filters("${ENGINE_SOURCES}")