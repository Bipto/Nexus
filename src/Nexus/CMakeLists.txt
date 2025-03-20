cmake_minimum_required(VERSION 3.16)

project ("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

if (NOT DEFINED NX_BUILD_TYPE)
  set(NX_BUILD_TYPE STATIC)
endif()

add_subdirectory(cmake-utils)

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

include (FetchContent)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG 4a9b579
)
FetchContent_MakeAvailable(SDL3)


FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 2d4c4b4
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG b1f4785
)
FetchContent_MakeAvailable(freetype)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG 5c20573
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 39f7374
)
FetchContent_MakeAvailable(yaml-cpp)

FetchContent_Declare(
  libnyquist
  GIT_REPOSITORY https://github.com/ddiakopoulos/libnyquist.git
  GIT_TAG 767efd9
)
FetchContent_MakeAvailable(libnyquist)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG 7945359
)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "")
FetchContent_MakeAvailable(assimp)

if (NOT EMSCRIPTEN)
  FetchContent_Declare(
    OpenAL-Soft
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG 8bcef34
  )
  FetchContent_MakeAvailable(OpenAL-Soft)
endif()

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 2f91176
)
FetchContent_GetProperties(imgui)
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)
if (NOT imgui_POPULATED)
  FetchContent_MakeAvailable(imgui)

  set (IMGUI_INCLUDE_DIR "${imgui_SOURCE_DIR}/")

  add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imconfig.h"
	  "${imgui_SOURCE_DIR}/imgui.cpp"
	  "${imgui_SOURCE_DIR}/imgui.h"
	  "${imgui_SOURCE_DIR}/imgui_demo.cpp"
	  "${imgui_SOURCE_DIR}/imgui_draw.cpp"
	  "${imgui_SOURCE_DIR}/imgui_internal.h"
	  "${imgui_SOURCE_DIR}/imgui_tables.cpp"
	  "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
	  "${imgui_SOURCE_DIR}/imstb_rectpack.h"
	  "${imgui_SOURCE_DIR}/imstb_textedit.h"
	  "${imgui_SOURCE_DIR}/imstb_truetype.h"	
	  "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
	  "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
  )

  target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR})
endif()

FetchContent_Declare(
  imguizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
  GIT_TAG b10e917
)
FetchContent_GetProperties(imguizmo)
if (NOT imguizmo_POPULATED)
  FetchContent_Populate(imguizmo)

  set (IMGUIZMO_INCLUDE_DIR "${imguizmo_SOURCE_DIR}/")

  add_library(imguizmo STATIC
    "${imguizmo_SOURCE_DIR}/ImGuizmo.h"
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp")

  target_include_directories(imguizmo PUBLIC ${IMGUI_INCLUDE_DIR} ${imguizmo_SOURCE_DIR})
endif()

FetchContent_Declare(spirv_cross
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
  GIT_TAG 104d91a
)
FetchContent_MakeAvailable(spirv_cross)

FetchContent_Declare(spirv_headers
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
  GIT_TAG a380cd2
)
FetchContent_MakeAvailable(spirv_headers)

FetchContent_Declare(spirv_tools
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
  GIT_TAG 8b39a8b
)
FetchContent_MakeAvailable(spirv_tools)

FetchContent_Declare(
  glslang
  GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
  GIT_TAG 9fe5223
)
FetchContent_MakeAvailable(glslang)

FetchContent_Declare(
  shaderc
  GIT_REPOSITORY https://github.com/google/shaderc.git
  GIT_TAG 4a8f5e5
)
FetchContent_MakeAvailable(shaderc)
if (WIN32)
  FetchContent_Declare(
    DirectXShaderCompiler
    URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2502/dxc_2025_02_20.zip
  )
  FetchContent_MakeAvailable(DirectXShaderCompiler)
endif()

set(NEXUS_INCLUDE_DIRS "")
set(NEXUS_LINKED_LIBRARIES "")
set(NEXUS_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_SHARED_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_COMPILE_DEFINITIONS "")

#platform specific configurations
if (WIN32)
  list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WINDOWS=1)
  set(NX_PLATFORM_WGL ON CACHE BOOL "")
elseif(LINUX)
  list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_LINUX=1)
elseif(EMSCRIPTEN)
  list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WEB=1)
elseif(ANDROID)

endif()

if (DEFINED NX_PLATFORM_OPENGL)
  find_package(OpenGL REQUIRED)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_OPENGL=1)
  list (APPEND NEXUS_LINKED_LIBRARIES ${OPENGL_LIBRARIES})

  if (WIN32)
      list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WGL=1)
      list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_GL_GLAD=1)
      list(APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_GL_DESKTOP=1)
      list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad_wgl)
      list(APPEND NEXUS_LINKED_LIBRARIES glad_wgl)

  elseif(LINUX OR ANDROID OR __ANDROID__)
    if (LINUX)
      list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_GL_DESKTOP=1)
    endif()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EGL REQUIRED egl)
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_EGL=1)
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_GL_GLAD=1)
    list (APPEND NEXUS_INCLUDE_DIRS ${EGL_INCLUDE_DIRS})
    list(APPEND NEXUS_LINKED_LIBRARIES glad_egl ${EGL_LIBRARIES})
    list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad_egl)

  elseif (EMSCRIPTEN)
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_WEBGL=1)
  endif()

endif()

if (DEFINED NX_PLATFORM_D3D12)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_D3D12=1)
endif()

if (DEFINED NX_PLATFORM_VULKAN)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_VULKAN=1)
  find_package(Vulkan REQUIRED)

  FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG 0e89587
  )
  FetchContent_MakeAvailable(VulkanMemoryAllocator)

  list (APPEND NEXUS_LINKED_LIBRARIES Vulkan::Vulkan VulkanMemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${VULKAN_INCLUDE_DIRS})
endif()

if (DEFINED NX_PLATFORM_OPENAL)
    list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_OPENAL=1)

    if (NOT EMSCRIPTEN)
      list (APPEND NEXUS_LINKED_LIBRARIES OpenAL)
    endif()
    
endif()

#external dependencies
list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad)

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
  file (GLOB_RECURSE NEXUS_OPENGL_SOURCES "src/Platform/OpenGL/*.cpp" "src/Platform/OpenGL/*.hpp")
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
    endif()

    if (LINUX)
      list (APPEND ENGINE_SOURCES src/Platform/Vulkan/Xlib/PlatformVkXlib.cpp)
      list (APPEND NEXUS_COMPILE_DEFINITIONS VK_USE_PLATFORM_XLIB_KHR=1)
    endif()
endif()

if (WIN32)
  file (GLOB_RECURSE NEXUS_WINDOWS_SOURCES "src/Platform/Windows/*cpp" "include/Platform/Windows/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_WINDOWS_SOURCES})
endif()

if (LINUX OR EMSCRIPTEN)
  file (GLOB_RECURSE NEXUS_X11_SOURCES "src/Platform/Unix/*.cpp" "include/Platform/Unix/*.hpp")
  list (APPEND ENGINE_SOURCES  ${NEXUS_X11_SOURCES})
endif()

add_library(Nexus ${NX_BUILD_TYPE} ${ENGINE_SOURCES})
nexus_setup_filters(${ENGINE_SOURCES})

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${stb_SOURCE_DIR}
)

list (APPEND NEXUS_LINKED_LIBRARIES 
  glad
  SDL3::SDL3
  assimp
  shaderc
  libnyquist
  spirv-cross-cpp
  spirv-cross-hlsl
  freetype
  yaml-cpp::yaml-cpp
  glm::glm
  imgui
  imguizmo)

#precompiled header
target_precompile_headers(Nexus PRIVATE include/Nexus-Core/nxpch.hpp)

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
nexus_add_all_shared_lib_subdirs("${NEXUS_SHARED_LIBRARY_SUBDIRECTORIES}")

set(NX_INCLUDE_VISIBILITY PUBLIC)
set(NX_LINK_VISIBILITY PUBLIC)
set(NX_COMPILE_DEFINITION_VISIBILITY PRIVATE)

target_include_directories(Nexus ${NX_INCLUDE_VISIBILITY} SYSTEM ${NEXUS_INCLUDE_DIRS})
target_link_libraries(Nexus ${NX_LINK_VISIBILITY} ${NEXUS_LINKED_LIBRARIES})
target_compile_definitions(Nexus ${NX_COMPILE_DEFINITION_VISIBILITY} ${NEXUS_COMPILE_DEFINITIONS})
nexus_structure_folders()
nexus_setup_filters("${ENGINE_SOURCES}")