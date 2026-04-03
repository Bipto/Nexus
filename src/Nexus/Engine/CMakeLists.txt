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
set(NEXUS_LINKED_LIBRARIES "")
set(NEXUS_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_COMPILE_DEFINITIONS "")

##################################################################################
#----------------------------------RenderDoc--------------------------------------
##################################################################################

set(RENDERDOC_VERSION "v1.32")
set(RENDERDOC_URL
    "https://raw.githubusercontent.com/baldurk/renderdoc/${RENDERDOC_VERSION}/renderdoc/api/app/renderdoc_app.h"
)
set(RENDERDOC_HEADER "${CMAKE_BINARY_DIR}/renderdoc/renderdoc_app.h")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/renderdoc")

if(NOT EXISTS "${RENDERDOC_HEADER}")
    message(STATUS "Downloading RenderDoc header from ${RENDERDOC_URL}")

    file(DOWNLOAD
        "${RENDERDOC_URL}"
        "${RENDERDOC_HEADER}"
        STATUS DOWNLOAD_STATUS
        SHOW_PROGRESS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download RenderDoc header: ${DOWNLOAD_STATUS}")
    endif()
else()
    message(STATUS "RenderDoc header already downloaded — skipping")
endif()


list (APPEND NEXUS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/renderdoc)

##################################################################################
#-------------------------------------ImGui---------------------------------------
##################################################################################

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)

CPMAddPackage(
	NAME imgui
	GITHUB_REPOSITORY ocornut/imgui
	GIT_TAG eb642a9
	OPTIONS
		"BUILD_TESTING OFF"
)

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

CPMAddPackage(
	NAME imguizmo
	GITHUB_REPOSITORY CedricGuillemet/ImGuizmo
	GIT_TAG b10e917
	OPTIONS
		"BUILD_TESTING OFF"
)

add_library(imguizmo
    "${imguizmo_SOURCE_DIR}/ImGuizmo.h"
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp"
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imguizmo PUBLIC ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR})

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/src/Nexus/external/glad2
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

  CPMAddPackage(
		NAME D3D12MemoryAllocator
		GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
		GIT_TAG v3.1.0
		OPTIONS
			"BUILD_TESTING OFF"
  )

  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_D3D12=1)
  list (APPEND NEXUS_LINKED_LIBRARIES D3D12MemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${D3D12MemoryAllocator_SOURCE_DIR}/include)

# Download DXC

  CPMAddPackage(
		NAME D3D12MemoryAllocator
		URL GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
  )

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
	list (APPEND NEXUS_LINKED_LIBARIES dxcompiler dxil)

	#pix
	CPMAddPackage(
		NAME WinPixEventRuntime
		URL https://www.nuget.org/api/v2/package/WinPixEventRuntime/1.0.230302001
	)

	# Paths inside the NuGet package
	set(PIX_INCLUDE_DIR ${WinPixEventRuntime_SOURCE_DIR}/Include)
	set(PIX_LIB_DIR ${WinPixEventRuntime_SOURCE_DIR}/bin/x64)

	# Create an imported library target
	add_library(WinPixEventRuntimeLib SHARED IMPORTED)

	set_target_properties(WinPixEventRuntimeLib PROPERTIES
		IMPORTED_LOCATION "${PIX_LIB_DIR}/WinPixEventRuntime.dll"
		IMPORTED_IMPLIB   "${PIX_LIB_DIR}/WinPixEventRuntime.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${PIX_INCLUDE_DIR}"
	)

	# Link your target against the imported library
	list (APPEND NEXUS_LINKED_LIBRARIES WinPixEventRuntimeLib)
endif()

if (NX_PLATFORM_VULKAN)
  list (APPEND NEXUS_COMPILE_DEFINITIONS NX_PLATFORM_VULKAN=1)
  find_package(Vulkan REQUIRED)

  CPMAddPackage(
	NAME VulkanMemoryAllocator
	GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
	GIT_TAG v3.3.0
	OPTIONS
		"BUILD_TESTING OFF"
)

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

CPMAddPackage(
	NAME stb_lib
	GITHUB_REPOSITORY nothings/stb
	GIT_TAG f1c79c0
)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_lib_SOURCE_DIR})
target_link_libraries(Nexus PUBLIC stb)

CPMAddPackage(
	NAME assimp
	GITHUB_REPOSITORY assimp/assimp
	GIT_TAG v6.0.4
	OPTIONS
		"ASSIMP_BUILD_ASSIMP_TOOLS OFF"
		"ASSIMP_BUILD_SAMPLES OFF"
		"ASSIMP_BUILD_TESTS OFF"
		"ASSIMP_BUILD_UNZIP OFF"
		"ASSIMP_BUILD_ZLIB ON"
)
target_link_libraries(Nexus PUBLIC assimp::assimp)

CPMAddPackage(
	NAME yaml_cpp
	GITHUB_REPOSITORY jbeder/yaml-cpp
	GIT_TAG yaml-cpp-0.9.0
)
target_link_libraries(Nexus PUBLIC yaml-cpp)

CPMAddPackage(
	NAME freetype
	GITHUB_REPOSITORY freetype/freetype
	GIT_TAG 85c8efe
)
target_link_libraries(Nexus PRIVATE freetype)

CPMAddPackage(
	NAME spirv-headers
	GITHUB_REPOSITORY KhronosGroup/SPIRV-Headers
	GIT_TAG vulkan-sdk-1.4.341.0
)

CPMAddPackage(
	NAME spirv-tools
	GITHUB_REPOSITORY KhronosGroup/SPIRV-Tools
	GIT_TAG v2026.1
	OPTIONS
		"SKIP_SPIRV_TOOLS_INSTALL ON"
		"ENABLE_SPIRV_TOOLS_INSTALL OFF"
)

CPMAddPackage(
	NAME glslang
	GITHUB_REPOSITORY KhronosGroup/glslang
	GIT_TAG 16.2.0
	OPTIONS
		"ENABLE_GLSLANG_BINARIES OFF"
		"SKIP_GLSLANG_INSTALL ON"
)
target_link_libraries(Nexus PRIVATE glslang)

CPMAddPackage(
	NAME shaderc
	GITHUB_REPOSITORY google/shaderc
	GIT_TAG v2026.1
	OPTIONS
		"SHADERC_ENABLE_TESTS OFF"
		"SHADERC_SKIP_EXAMPLES ON"
		"SHADERC_SKIP_TESTS ON"
		"SHADERC_ENABLE_SHARED_CRT ON"
)
target_link_libraries(Nexus PRIVATE shaderc)

CPMAddPackage(
	NAME spirv-cross
	GITHUB_REPOSITORY KhronosGroup/SPIRV-Cross
	GIT_TAG a0fba56
	OPTIONS
		"SPIRV_CROSS_CLI OFF"
		"SPIRV_CROSS_ENABLE_TESTS ON"
		"SPIRV_SKIP_EXECUTABLES ON"
		"SPIRV_SKIP_TESTS ON"
		"SPIRV_CROSS_ENABLE_UTIL OFF"
)
target_link_libraries(Nexus PRIVATE spirv-cross-cpp spirv-cross-hlsl)

CPMAddPackage(
	NAME spirv_reflect
	GITHUB_REPOSITORY KhronosGroup/SPIRV-Reflect
	GIT_TAG vulkan-sdk-1.4.341.0
	OPTIONS
		"SPIRV_REFLECT_EXECUTABLE OFF"
		"SPIRV_REFLECT_STATIC_LIB ON"
)
target_link_libraries(Nexus PRIVATE spirv-reflect-static)

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list (APPEND NEXUS_LINKED_LIBRARIES
  assimp::assimp
  Nexus-Core
  Nexus-Audio
  Nexus-Platform
  Nexus-RHI
  shaderc
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

if (BUILD_TESTING)
	add_subdirectory(tests)
endif()

