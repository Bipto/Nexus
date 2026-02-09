cmake_minimum_required(VERSION 3.28)

project ("Nexus")

include(FetchContent)

if (NOT DEFINED NX_BUILD_TYPE OR EMSCRIPTEN)
  set(NX_BUILD_TYPE STATIC)
endif()

if (NX_BUILD_TYPE STREQUAL "SHARED")
  add_compile_definitions(NX_BUILD_DLL=1)
  add_compile_definitions(NX_EXPORT_API=1)
endif()

add_subdirectory(cmake)
add_subdirectory(Audio)
add_subdirectory(Platform)

set(NEXUS_INCLUDE_DIRS "external/renderdoc")
set(NEXUS_LINKED_LIBRARIES "")
set(NEXUS_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_COMPILE_DEFINITIONS "")

#fetch renderdoc header
set(RENDERDOC_VERSION "v1.32")
set(RENDERDOC_URL "https://raw.githubusercontent.com/baldurk/renderdoc/${RENDERDOC_VERSION}/renderdoc/api/app/renderdoc_app.h")
set(RENDERDOC_HEADER "${CMAKE_BINARY_DIR}/renderdoc_app.h")

if(NOT EXISTS "${RENDERDOC_HEADER}")
    message(STATUS "Downloading RenderDoc header from ${RENDERDOC_URL}")
    file(DOWNLOAD "${RENDERDOC_URL}" "${RENDERDOC_HEADER}" SHOW_PROGRESS)
else()
    message(STATUS "RenderDoc header already downloaded � skipping")
endif()

list (APPEND NEXUS_INCLUDE_DIRS ${CMAKE_BINARY_DIR})

list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/imgui)
list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad2)

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

if (NX_PLATFORM_OPENGL)
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

if (NX_PLATFORM_D3D12)
	
  FetchContent_Declare(
	  D3D12MemoryAllocator
	  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
	  GIT_TAG c06cd9a
	  GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(D3D12MemoryAllocator)

  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_D3D12=1)
  list (APPEND NEXUS_LINKED_LIBRARIES D3D12MemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${D3D12MemoryAllocator_SOURCE_DIR}/include)
endif()

if (NX_PLATFORM_VULKAN)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_VULKAN=1)
  find_package(Vulkan REQUIRED)

  FetchContent_Declare(
	  VulkanMemoryAllocator
	  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
	  GIT_TAG e722e57
	  GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(VulkanMemoryAllocator)

  list (APPEND NEXUS_COMPILE_DEFINITIONS VK_STATIC_VULKAN_FUNCTIONS=1)

  list (APPEND NEXUS_LINKED_LIBRARIES Vulkan::Vulkan VulkanMemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${VULKAN_INCLUDE_DIRS})
endif()

#external dependencies

set(ENGINE_SOURCES "include/Nexus.hpp" "src/Nexus.cpp")
file (GLOB_RECURSE NEXUS_SOURCES "include/*.hpp" "src/Nexus-Core/*.cpp")
list (APPEND ENGINE_SOURCES ${NEXUS_SOURCES})

if (NX_PLATFORM_D3D12)
  file (GLOB_RECURSE NEXUS_D3D12_SOURCES "src/Platform/D3D12/*.cpp" "src/Platform/D3D12/*.hpp" "src/Platform/D3D12/Surface/*.cpp" "src/Platform/D3D12/Surface/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_D3D12_SOURCES})
endif()

if (NX_PLATFORM_OPENGL)
  file (GLOB NEXUS_OPENGL_SOURCES "src/Platform/OpenGL/*.cpp" "src/Platform/OpenGL/*.hpp" "src/Platform/OpenGL/Surface/*.cpp" "src/Platform/OpenGL/Surface/*.hpp")

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

if (NX_PLATFORM_EGL)
  file (GLOB_RECURSE NEXUS_EGL_SOURCES "src/Platform/OpenGL/Context/EGL/*.cpp" "src/Platform/OpenGL/Context/EGL/*.hpp" "src/Platform/OpenGL/Surface/EGL/*.cpp" "src/Platform/OpenGL/Surface/EGL/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_EGL_SOURCES})
endif()

if (NX_PLATFORM_WEBGL)
  file (GLOB_RECURSE NEXUS_WEBGL_SOURCES "src/Platform/OpenGL/Context/WebGL/*.cpp" "src/Platform/OpenGL/Context/WebGL/*.hpp")
  list (APPEND ENGINE_SOURCES ${NEXUS_WEBGL_SOURCES})
endif()

if (NX_PLATFORM_WGL)
	file (GLOB_RECURSE NEXUS_WGL_SOURCES "src/Platform/OpenGL/Context/WGL/*.cpp" "src/Platform/OpenGL/Context/WGL/*.hpp" "src/Platform/OpenGL/Surface/WGL/*.cpp" "src/Platform/OpenGL/Surface/WGL/*.hpp")
	list (APPEND ENGINE_SOURCES ${NEXUS_WGL_SOURCES})
endif()

file (GLOB_RECURSE NEXUS_SDL_SOURCES "src/Platform/SDL3/*.cpp" "src/Platform/SDL3/*.hpp")
list (APPEND ENGINE_SOURCES ${NEXUS_SDL_SOURCES})

if (NX_PLATFORM_VULKAN)
	file (GLOB NEXUS_VULKAN_SOURCES "src/Platform/Vulkan/*.cpp" "src/Platform/Vulkan/*.hpp")

  list (APPEND NEXUS_VULKAN_SOURCES "src/Platform/Vulkan/Surface/SurfaceVk.hpp")
  
  if (WIN32)
    list (APPEND NEXUS_VULKAN_SOURCES "src/Platform/Vulkan/Surface/SurfaceWin32_Vk.hpp" "src/Platform/Vulkan/Surface/SurfaceWin32_Vk.cpp")
  endif()

  if (LINUX)
    list (APPEND NEXUS_VULKAN_SOURCES "src/Platform/Vulkan/Surface/SurfaceX11_Vk.hpp" "src/Platform/Vulkan/Surface/SurfaceX11_Vk.cpp")
  endif()

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

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list (APPEND NEXUS_LINKED_LIBRARIES
  Nexus-Audio
  Nexus-Platform
  imgui
  imguizmo
  glad2)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)
target_link_libraries(Nexus PUBLIC glm::glm)

#precompiled header
#target_precompile_headers(Nexus PRIVATE include/Nexus-Core/nxpch.hpp)

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

set(NX_INCLUDE_VISIBILITY PUBLIC)
set(NX_LINK_VISIBILITY PUBLIC)
set(NX_COMPILE_DEFINITION_VISIBILITY PUBLIC)#

FetchContent_Declare(
	stb_lib
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG f1c79c0
)
FetchContent_MakeAvailable(stb_lib)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_lib_SOURCE_DIR})
target_link_libraries(Nexus PUBLIC stb)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_UNZIP OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	GIT_TAG v6.0.4
)
FetchContent_MakeAvailable(assimp)
target_link_libraries(Nexus PUBLIC assimp::assimp)

FetchContent_Declare(
	yaml-cpp
	GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
	GIT_TAG yaml-cpp-0.9.0
)
FetchContent_MakeAvailable(yaml-cpp)
target_link_libraries(Nexus PUBLIC yaml-cpp)

FetchContent_Declare(
	freetype
	GIT_REPOSITORY https://github.com/freetype/freetype.git
	GIT_TAG 85c8efe
)
FetchContent_MakeAvailable(freetype)
target_link_libraries(Nexus PRIVATE freetype)

FetchContent_Declare(
	spirv-headers
	GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
	GIT_TAG vulkan-sdk-1.4.341.0
)
FetchContent_MakeAvailable(spirv-headers)

set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_SPIRV_TOOLS_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
	spirv-tools
	GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
	GIT_TAG v2026.1
)
FetchContent_MakeAvailable(spirv-tools)

set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(
	glslang
	GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
	GIT_TAG 16.2.0
)
FetchContent_MakeAvailable(glslang)
target_link_libraries(Nexus PRIVATE glslang)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(SHADERC_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_ENABLE_SHARED_CRT ON)
FetchContent_Declare(
	shaderc
	GIT_REPOSITORY https://github.com/google/shaderc.git
	GIT_TAG v2026.1
)
FetchContent_MakeAvailable(shaderc)
target_link_libraries(Nexus PRIVATE shaderc)

set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
	spirv-cross
	GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
	GIT_TAG a0fba56
)
FetchContent_MakeAvailable(spirv-cross)
target_link_libraries(Nexus PRIVATE spirv-cross-cpp spirv-cross-hlsl)

set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "" FORCE)

FetchContent_Declare(
	spirv_reflect
	GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
	GIT_TAG vulkan-sdk-1.4.341.0
)
FetchContent_MakeAvailable(spirv_reflect)
target_link_libraries(Nexus PRIVATE spirv-reflect-static)

target_include_directories(Nexus ${NX_INCLUDE_VISIBILITY} SYSTEM ${NEXUS_INCLUDE_DIRS})
target_link_libraries(Nexus ${NX_LINK_VISIBILITY} ${NEXUS_LINKED_LIBRARIES})
target_compile_definitions(Nexus ${NX_COMPILE_DEFINITION_VISIBILITY} ${NEXUS_COMPILE_DEFINITIONS})
nexus_structure_folders()
nexus_setup_filters("${ENGINE_SOURCES}")

if (NX_PLATFORM_D3D12)
	# Download DXC
	FetchContent_Declare(
		dxc
		URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505.1/dxc_2025_07_14.zip
	)

	FetchContent_MakeAvailable(dxc)

	# DXC root folder
	set(DXC_ROOT ${dxc_SOURCE_DIR})

	set(DXC_INCLUDE_DIR ${DXC_ROOT}/inc)
	set(DXC_LIB_DIR     ${DXC_ROOT}/lib/x64)
	set(DXC_BIN_DIR     ${DXC_ROOT}/bin/x64)

	# Create imported target for dxcompiler.dll
	add_library(dxcompiler SHARED IMPORTED)

	set_target_properties(dxcompiler PROPERTIES
		IMPORTED_LOCATION     "${DXC_BIN_DIR}/dxcompiler.dll"
		IMPORTED_IMPLIB       "${DXC_LIB_DIR}/dxcompiler.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${DXC_INCLUDE_DIR}"
	)

	# Optional: DXIL library (needed for SM6.x validation)
	add_library(dxil SHARED IMPORTED)

	set_target_properties(dxil PROPERTIES
		IMPORTED_LOCATION     "${DXC_BIN_DIR}/dxil.dll"
		IMPORTED_IMPLIB       "${DXC_LIB_DIR}/dxil.lib"
	)

	# Link your engine
	target_link_libraries(Nexus PRIVATE dxcompiler dxil)

	#pix
	FetchContent_Declare(
		WinPixEventRuntime
		URL https://www.nuget.org/api/v2/package/WinPixEventRuntime/1.0.230302001
		DOWNLOAD_NO_EXTRACT FALSE
	)

	FetchContent_MakeAvailable(WinPixEventRuntime)

	# Paths inside the NuGet package
	set(PIX_INCLUDE_DIR ${winpixeventruntime_SOURCE_DIR}/Include)
	set(PIX_LIB_DIR ${winpixeventruntime_SOURCE_DIR}/bin/x64)

	# Create an imported library target
	add_library(WinPixEventRuntimeLib SHARED IMPORTED)

	set_target_properties(WinPixEventRuntimeLib PROPERTIES
		IMPORTED_LOCATION "${PIX_LIB_DIR}/WinPixEventRuntime.dll"
		IMPORTED_IMPLIB   "${PIX_LIB_DIR}/WinPixEventRuntime.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${PIX_INCLUDE_DIR}"
	)

	# Link your target against the imported library
	target_link_libraries(Nexus PRIVATE WinPixEventRuntimeLib)

endif()

target_compile_features(Nexus PUBLIC cxx_std_20)
set_target_properties(Nexus PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR OFF)