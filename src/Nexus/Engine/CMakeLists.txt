cmake_minimum_required(VERSION 3.28)

include(FetchContent)

if (NOT DEFINED NX_BUILD_TYPE OR EMSCRIPTEN)
  set(NX_BUILD_TYPE STATIC)
endif()

if (NX_BUILD_TYPE STREQUAL "SHARED")
  add_compile_definitions(NX_BUILD_DLL=1)
  add_compile_definitions(NX_EXPORT_API=1)
endif()

set(NEXUS_INCLUDE_DIRS "")
set(NEXUS_LINKED_LIBRARIES "")
set(NEXUS_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_COMPILE_DEFINITIONS "")

find_package(assimp CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_hlsl CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

##################################################################################
#----------------------------------RenderDoc--------------------------------------
##################################################################################

set(RENDERDOC_VERSION "v1.32")
set(RENDERDOC_URL "https://raw.githubusercontent.com/baldurk/renderdoc/${RENDERDOC_VERSION}/renderdoc/api/app/renderdoc_app.h")
set(RENDERDOC_HEADER "${CMAKE_BINARY_DIR}/renderdoc/renderdoc_app.h")

if(NOT EXISTS "${RENDERDOC_HEADER}")
    message(STATUS "Downloading RenderDoc header from ${RENDERDOC_URL}")
    file(DOWNLOAD "${RENDERDOC_URL}" "${RENDERDOC_HEADER}" SHOW_PROGRESS)
else()
    message(STATUS "RenderDoc header already downloaded � skipping")
endif()

list (APPEND NEXUS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/renderdoc)

##################################################################################
#-------------------------------------ImGui---------------------------------------
##################################################################################

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG eb642a9
)
FetchContent_MakeAvailable(imgui)

set (Files
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
	"${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp")

add_library(imgui
	${Files}
)

##################################################################################
#-----------------------------------ImGuizmo--------------------------------------
##################################################################################

FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG b10e917
)
FetchContent_MakeAvailable(imguizmo)

add_library(imguizmo
    "${imguizmo_SOURCE_DIR}/ImGuizmo.h"
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imguizmo PUBLIC ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR})

add_subdirectory(
    ${PROJECT_SOURCE_DIR}/external/glad2
    ${CMAKE_BINARY_DIR}/external/glad2
)

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
	  GIT_TAG v3.1.0
	  GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(D3D12MemoryAllocator)

  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_D3D12=1)
  list (APPEND NEXUS_LINKED_LIBRARIES D3D12MemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${D3D12MemoryAllocator_SOURCE_DIR}/include)

  find_package(winpixevent CONFIG REQUIRED)
  list (APPEND NEXUS_LINKED_LIBRARIES Microsoft::WinPixEventRuntime)
endif()

if (NX_PLATFORM_VULKAN)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_VULKAN=1)
  find_package(Vulkan REQUIRED)

  FetchContent_Declare(
	  VulkanMemoryAllocator
	  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
	  GIT_TAG v3.3.0
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
  assimp::assimp
  Nexus-Audio
  Nexus-Platform
  Nexus-RHI
  unofficial::shaderc::shaderc
  spirv-cross-core
  spirv-cross-glsl
  spirv-cross-hlsl
  freetype
  yaml-cpp::yaml-cpp
  glm::glm
  imgui
  imguizmo
  glad2)

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
set(NX_COMPILE_DEFINITION_VISIBILITY PUBLIC)

target_include_directories(Nexus ${NX_INCLUDE_VISIBILITY} SYSTEM ${NEXUS_INCLUDE_DIRS})
target_link_libraries(Nexus ${NX_LINK_VISIBILITY} ${NEXUS_LINKED_LIBRARIES})
target_compile_definitions(Nexus ${NX_COMPILE_DEFINITION_VISIBILITY} ${NEXUS_COMPILE_DEFINITIONS})
nexus_structure_folders()
nexus_setup_filters("${ENGINE_SOURCES}")

target_compile_features(Nexus PUBLIC cxx_std_23)
set_target_properties(Nexus PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR OFF)

if (DEFINED BUILD_TESTING)
	add_subdirectory(tests)
endif()
