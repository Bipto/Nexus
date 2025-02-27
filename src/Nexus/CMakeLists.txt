cmake_minimum_required(VERSION 3.16)

add_subdirectory(cmake)
project ("Nexus")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)
add_definitions(-DNX_EXPORT_API=1)

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
    GIT_TAG c14554f
  )
  FetchContent_MakeAvailable(OpenAL-Soft)
endif()

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 2f91176
)
FetchContent_GetProperties(imgui)
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

  target_include_directories(imguizmo PUBLIC ${IMGUI_INCLUDE_DIR})
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

FetchContent_Declare(
  pocketpy
  GIT_REPOSITORY https://github.com/pocketpy/pocketpy.git
  GIT_TAG 3f5c460
)
FetchContent_MakeAvailable(pocketpy)

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
list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES 
  external/glad)

set (ENGINE_SOURCES
  src/Nexus.cpp
  include/Nexus.hpp
  src/Nexus-Core/Application.cpp
  include/Nexus-Core/Application.hpp
  include/Nexus-Core/ApplicationSpecification.hpp
  src/Nexus-Core/AssetManager.cpp
  include/Nexus-Core/AssetManager.hpp
  include/Nexus-Core/IView.hpp
  include/Nexus-Core/Monitor.hpp
  src/Nexus-Core/nxpch.cpp
  include/Nexus-Core/nxpch.hpp
  include/Nexus-Core/Platform.hpp
  include/Nexus-Core/Point.hpp
  src/Nexus-Core/Runtime.cpp
  include/Nexus-Core/Runtime.hpp
  include/Nexus-Core/Types.hpp
  src/Nexus-Core/Vertex.cpp
  include/Nexus-Core/Vertex.hpp
  include/Nexus-Core/IWindow.hpp
  include/Nexus-Core/WindowInfo.hpp
)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Audio/AudioBuffer.hpp
  include/Nexus-Core/Audio/AudioDevice.hpp
  include/Nexus-Core/Audio/AudioSource.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Caching/CachedShader.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/ECS/ComponentRegistry.hpp
  include/Nexus-Core/ECS/Registry.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Events/EventHandler.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/FileSystem/FileDialogs.hpp
  src/Nexus-Core/FileSystem/FileSystem.cpp
  include/Nexus-Core/FileSystem/FileSystem.hpp)

list (APPEND ENGINE_SOURCES 
  
  include/Nexus-Core/Graphics/CatmullRom.hpp
  include/Nexus-Core/Graphics/Circle.hpp
  include/Nexus-Core/Graphics/Color.hpp
  src/Nexus-Core/Graphics/CommandExecutor.cpp
  include/Nexus-Core/Graphics/CommandExecutor.hpp
  src/Nexus-Core/Graphics/CommandList.cpp
  include/Nexus-Core/Graphics/CommandList.hpp
  src/Nexus-Core/Graphics/Font.cpp
  include/Nexus-Core/Graphics/Font.hpp
  include/Nexus-Core/Graphics/Framebuffer.hpp
  src/Nexus-Core/Graphics/FullscreenQuad.cpp
  include/Nexus-Core/Graphics/FullscreenQuad.hpp
  include/Nexus-Core/Graphics/GPUBuffer.hpp
  include/Nexus-Core/Graphics/GraphicsCapabilities.hpp
  src/Nexus-Core/Graphics/GraphicsDevice.cpp
  include/Nexus-Core/Graphics/GraphicsDevice.hpp
  src/Nexus-Core/Graphics/HdriProcessor.cpp
  include/Nexus-Core/Graphics/HdriProcessor.hpp
  include/Nexus-Core/Graphics/IResource.hpp
  include/Nexus-Core/Graphics/Mesh.hpp
  src/Nexus-Core/Graphics/MeshFactory.cpp
  include/Nexus-Core/Graphics/MeshFactory.hpp
  src/Nexus-Core/Graphics/MipmapGenerator.cpp
  include/Nexus-Core/Graphics/MipmapGenerator.hpp
  include/Nexus-Core/Graphics/Model.hpp
  src/Nexus-Core/Graphics/Multisample.cpp
  include/Nexus-Core/Graphics/Multisample.hpp
  include/Nexus-Core/Graphics/Pipeline.hpp
  include/Nexus-Core/Graphics/PixelFormat.hpp
  include/Nexus-Core/Graphics/Polygon.hpp
  include/Nexus-Core/Graphics/Rectangle.hpp
  include/Nexus-Core/Graphics/RenderTarget.hpp
  src/Nexus-Core/Graphics/ResourceSet.cpp
  include/Nexus-Core/Graphics/ResourceSet.hpp
  include/Nexus-Core/Graphics/RoundedRectangle.hpp
  include/Nexus-Core/Graphics/Sampler.hpp
  include/Nexus-Core/Graphics/SamplerState.hpp
  include/Nexus-Core/Graphics/Scissor.hpp
  src/Nexus-Core/Graphics/ShaderDataType.cpp
  include/Nexus-Core/Graphics/ShaderDataType.hpp
  src/Nexus-Core/Graphics/ShaderGenerator.cpp
  include/Nexus-Core/Graphics/ShaderGenerator.hpp
  include/Nexus-Core/Graphics/ShaderModule.hpp
  include/Nexus-Core/Graphics/ShaderUtils.hpp
  src/Nexus-Core/Graphics/stb.cpp
  include/Nexus-Core/Graphics/Structures.hpp
  include/Nexus-Core/Graphics/Swapchain.hpp
  src/Nexus-Core/Graphics/Texture.cpp
  include/Nexus-Core/Graphics/Texture.hpp
  include/Nexus-Core/Graphics/TimingQuery.hpp
  include/Nexus-Core/Graphics/Triangle.hpp
  include/Nexus-Core/Graphics/Viewport.hpp
  include/Nexus-Core/Graphics/WindingOrder.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Importers/AssimpImporter.hpp
  src/Nexus-Core/Importers/AssimpImporter.cpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/ImGui/ImGuiGraphicsRenderer.cpp
  include/Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp
  include/Nexus-Core/ImGui/ImGuiInclude.hpp  
)

list (APPEND ENGINE_SOURCES
include/Nexus-Core/Input/Event.hpp
  include/Nexus-Core/Input/Gamepad.hpp
  src/Nexus-Core/Input/Input.cpp
  include/Nexus-Core/Input/Input.hpp
  src/Nexus-Core/Input/InputContext.cpp
  include/Nexus-Core/Input/InputContext.hpp
  src/Nexus-Core/Input/InputState.cpp
  include/Nexus-Core/Input/InputState.hpp
  src/Nexus-Core/Input/Keyboard.cpp
  include/Nexus-Core/Input/Keyboard.hpp
  src/Nexus-Core/Input/Mouse.cpp
  include/Nexus-Core/Input/Mouse.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Layers/Layer.hpp
  src/Nexus-Core/Layers/LayerStack.cpp
  include/Nexus-Core/Layers/LayerStack.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Logging/Log.cpp
  include/Nexus-Core/Logging/Log.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Renderer/BatchRenderer.cpp
  include/Nexus-Core/Renderer/BatchRenderer.hpp
  src/Nexus-Core/Renderer/Renderer3D.cpp
  include/Nexus-Core/Renderer/Renderer3D.hpp
)

list (APPEND ENGINE_SOURCES
include/Nexus-Core/Runtime/Camera.hpp
  src/Nexus-Core/Runtime/Entity.cpp
  include/Nexus-Core/Runtime/Entity.hpp
  src/Nexus-Core/Runtime/Project.cpp
  include/Nexus-Core/Runtime/Project.hpp
  src/Nexus-Core/Runtime/Scene.cpp
  include/Nexus-Core/Runtime/Scene.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Scripting/NativeScript.hpp
  include/Nexus-Core/Scripting/PythonScript.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Timings/Clock.hpp
  src/Nexus-Core/Timings/Profiler.cpp
  include/Nexus-Core/Timings/Profiler.hpp
  include/Nexus-Core/Timings/Timer.hpp
  include/Nexus-Core/Timings/Timespan.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/UI/Canvas.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Utils/FramerateMonitor.hpp
  include/Nexus-Core/Utils/GUID.hpp
  src/Nexus-Core/Utils/Utils.cpp
  include/Nexus-Core/Utils/Utils.hpp
  include/Nexus-Core/Utils/ScriptProjectGenerator.hpp
  src/Nexus-Core/Utils/ScriptProjectGenerator.cpp
  include/Nexus-Core/Utils/SharedLibrary.hpp
  src/Nexus-Core/Utils/StringUtils.cpp
  include/Nexus-Core/Utils/StringUtils.hpp)

if (DEFINED NX_PLATFORM_D3D12)
  list (APPEND ENGINE_SOURCES
    src/Platform/D3D12/BufferD3D12.cpp
    src/Platform/D3D12/BufferD3D12.hpp
    src/Platform/D3D12/CommandExecutorD3D12.cpp
    src/Platform/D3D12/CommandExecutorD3D12.hpp
    src/Platform/D3D12/CommandListD3D12.cpp
    src/Platform/D3D12/CommandListD3D12.hpp
    src/Platform/D3D12/D3D12Include.hpp
    src/Platform/D3D12/D3D12Utils.cpp
    src/Platform/D3D12/D3D12Utils.hpp
    src/Platform/D3D12/FramebufferD3D12.cpp
    src/Platform/D3D12/FramebufferD3D12.hpp
    src/Platform/D3D12/GraphicsDeviceD3D12.cpp
    src/Platform/D3D12/GraphicsDeviceD3D12.hpp
    src/Platform/D3D12/PipelineD3D12.cpp
    src/Platform/D3D12/PipelineD3D12.hpp
    src/Platform/D3D12/ResourceSetD3D12.cpp
    src/Platform/D3D12/ResourceSetD3D12.hpp
    src/Platform/D3D12/SamplerD3D12.cpp
    src/Platform/D3D12/SamplerD3D12.hpp
    src/Platform/D3D12/ShaderModuleD3D12.cpp
    src/Platform/D3D12/ShaderModuleD3D12.hpp
    src/Platform/D3D12/SwapchainD3D12.cpp
    src/Platform/D3D12/SwapchainD3D12.hpp
    src/Platform/D3D12/TextureD3D12.cpp
    src/Platform/D3D12/TextureD3D12.hpp
    src/Platform/D3D12/TimingQueryD3D12.cpp
    src/Platform/D3D12/TimingQueryD3D12.hpp)
  endif()

if (DEFINED NX_PLATFORM_OPENAL)
  list (APPEND ENGINE_SOURCES
    src/Platform/OpenAL/AudioBufferOpenAL.cpp
    src/Platform/OpenAL/AudioBufferOpenAL.hpp
    src/Platform/OpenAL/AudioDeviceOpenAL.cpp
    src/Platform/OpenAL/AudioDeviceOpenAL.hpp
    src/Platform/OpenAL/AudioSourceOpenAL.cpp
    src/Platform/OpenAL/AudioSourceOpenAL.hpp
    src/Platform/OpenAL/OpenAL.hpp)
endif()

if (DEFINED NX_PLATFORM_OPENGL)
  list (APPEND ENGINE_SOURCES
    src/Platform/OpenGL/BufferOpenGL.cpp
    src/Platform/OpenGL/BufferOpenGL.hpp
    src/Platform/OpenGL/CommandExecutorOpenGL.cpp
    src/Platform/OpenGL/CommandExecutorOpenGL.hpp
    src/Platform/OpenGL/CommandListOpenGL.hpp
    src/Platform/OpenGL/ContextSpecification.hpp
    src/Platform/OpenGL/FramebufferOpenGL.cpp
    src/Platform/OpenGL/FramebufferOpenGL.hpp
    src/Platform/OpenGL/GL.cpp
    src/Platform/OpenGL/GL.hpp
    src/Platform/OpenGL/GraphicsDeviceOpenGL.cpp
    src/Platform/OpenGL/GraphicsDeviceOpenGL.hpp
    src/Platform/OpenGL/PipelineOpenGL.cpp
    src/Platform/OpenGL/PipelineOpenGL.hpp
    src/Platform/OpenGL/ResourceSetOpenGL.cpp
    src/Platform/OpenGL/ResourceSetOpenGL.hpp
    src/Platform/OpenGL/SamplerOpenGL.cpp
    src/Platform/OpenGL/SamplerOpenGL.hpp
    src/Platform/OpenGL/ShaderModuleOpenGL.cpp
    src/Platform/OpenGL/ShaderModuleOpenGL.hpp
    src/Platform/OpenGL/SwapchainOpenGL.cpp
    src/Platform/OpenGL/SwapchainOpenGL.hpp
    src/Platform/OpenGL/TextureOpenGL.cpp
    src/Platform/OpenGL/TextureOpenGL.hpp
    src/Platform/OpenGL/TimingQueryOpenGL.cpp
    src/Platform/OpenGL/TimingQueryOpenGL.hpp)
endif()

list (APPEND ENGINE_SOURCES
  src/Platform/OpenGL/Context/IOffscreenContext.hpp
  src/Platform/OpenGL/Context/IViewContext.hpp)

if (DEFINED NX_PLATFORM_EGL)
  list (APPEND ENGINE_SOURCES
    src/Platform/OpenGL/Context/EGL/EGLUtils.hpp
    src/Platform/OpenGL/Context/EGL/OffscreenContextEGL.cpp
    src/Platform/OpenGL/Context/EGL/OffscreenContextEGL.hpp
    src/Platform/OpenGL/Context/EGL/ViewContextEGL.cpp
    src/Platform/OpenGL/Context/EGL/ViewContextEGL.hpp)
endif()

if (DEFINED NX_PLATFORM_WEBGL)
  list (APPEND ENGINE_SOURCES
    src/Platform/OpenGL/Context/WebGL/OffscreenContextWebGL.cpp
    src/Platform/OpenGL/Context/WebGL/OffscreenContextWebGL.hpp
    src/Platform/OpenGL/Context/WebGL/ViewContextWebGL.cpp
    src/Platform/OpenGL/Context/WebGL/ViewContextWebGL.hpp)
endif()

if (DEFINED NX_PLATFORM_WGL)
  list (APPEND ENGINE_SOURCES
    src/Platform/OpenGL/Context/WGL/OffscreenContextWGL.cpp
    src/Platform/OpenGL/Context/WGL/OffscreenContextWGL.hpp
    src/Platform/OpenGL/Context/WGL/ViewContextWGL.cpp
    src/Platform/OpenGL/Context/WGL/ViewContextWGL.hpp
    )
endif()

list (APPEND ENGINE_SOURCES
  src/Platform/SDL3/FileDialogsSDL3.cpp
  src/Platform/SDL3/FileDialogsSDL3.hpp
  src/Platform/SDL3/GamepadSDL3.cpp
  src/Platform/SDL3/GamepadSDL3.hpp
  src/Platform/SDL3/SDL3Include.cpp
  src/Platform/SDL3/SDL3Include.hpp
  src/Platform/SDL3/SDL3Window.cpp
  src/Platform/SDL3/SDL3Window.hpp
  src/Platform/SDL3/SDLPlatform.cpp)

if (DEFINED NX_PLATFORM_VULKAN)
  list (APPEND ENGINE_SOURCES
    src/Platform/Vulkan/BufferVk.cpp
    src/Platform/Vulkan/BufferVk.hpp
    src/Platform/Vulkan/CommandExecutorVk.cpp
    src/Platform/Vulkan/CommandExecutorVk.hpp
    src/Platform/Vulkan/CommandListVk.cpp
    src/Platform/Vulkan/CommandListVk.hpp
    src/Platform/Vulkan/FramebufferVk.cpp
    src/Platform/Vulkan/FramebufferVk.hpp
    src/Platform/Vulkan/GraphicsDeviceVk.cpp
    src/Platform/Vulkan/GraphicsDeviceVk.hpp
    src/Platform/Vulkan/PipelineVk.cpp
    src/Platform/Vulkan/PipelineVk.hpp
    src/Platform/Vulkan/PlatformVk.hpp
    src/Platform/Vulkan/ResourceSetVk.cpp
    src/Platform/Vulkan/ResourceSetVk.hpp
    src/Platform/Vulkan/SamplerVk.cpp
    src/Platform/Vulkan/SamplerVk.hpp
    src/Platform/Vulkan/ShaderModuleVk.cpp
    src/Platform/Vulkan/ShaderModuleVk.hpp
    src/Platform/Vulkan/SwapchainVk.cpp
    src/Platform/Vulkan/SwapchainVk.hpp
    src/Platform/Vulkan/TextureVk.cpp
    src/Platform/Vulkan/TextureVk.hpp
    src/Platform/Vulkan/TimingQueryVk.cpp
    src/Platform/Vulkan/TimingQueryVk.hpp
    src/Platform/Vulkan/Vk.cpp
    src/Platform/Vulkan/Vk.hpp)

    if (WIN32)
      list (APPEND ENGINE_SOURCES
        src/Platform/Vulkan/Windows/PlatformVkWindows.cpp)
        list (APPEND NEXUS_COMPILE_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR=1)
    endif()

    if (LINUX)
      list (APPEND ENGINE_SOURCES
        src/Platform/Vulkan/Xlib/PlatformVkXlib.cpp)
        list (APPEND NEXUS_COMPILE_DEFINITIONS VK_USE_PLATFORM_XLIB_KHR=1)
    endif()
endif()

if (WIN32)
  list (APPEND ENGINE_SOURCES
    src/Platform/Windows/PlatformWindows.cpp
    src/Platform/Windows/SharedLibraryWindows.hpp
    src/Platform/Windows/SharedLibraryWindows.cpp
    include/Platform/Windows/WindowsInclude.hpp)
endif()

if (LINUX)
  list (APPEND ENGINE_SOURCES include/Platform/X11/X11Include.hpp)
endif()

if (NOT DEFINED NX_BUILD_TYPE)
  set(NX_BUILD_TYPE STATIC)
endif()

add_library(Nexus ${NX_BUILD_TYPE} ${ENGINE_SOURCES})
nexus_setup_filters(${ENGINE_SOURCES})

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${stb_SOURCE_DIR}
  ${IMGUI_INCLUDE_DIR}
  ${IMGUIZMO_INCLUDE_DIR}
  ${pocketpy_SOURCE_DIR}/include
  binaries/dxc_2023_08_14/inc
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
  imguizmo
  pocketpy)

#precompiled header
if (NX_ROOT_BUILD STREQUAL "ON")
  target_precompile_headers(Nexus PRIVATE include/Nexus-Core/nxpch.hpp)
endif()

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Nexus
        PROPERTIES
        LINK_FLAGS
        "--std=c++20 -O3 -flto -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS"
    )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lidbfs.js")
    target_link_options(Nexus PRIVATE -lidbfs.js)
endif()

nexus_add_all_subdirs("${NEXUS_LIBRARY_SUBDIRECTORIES}")
nexus_add_all_shared_lib_subdirs("${NEXUS_SHARED_LIBRARY_SUBDIRECTORIES}")

set(NX_INCLUDE_VISIBILITY PUBLIC)
set(NX_LINK_VISIBILITY PUBLIC)
set(NX_COMPILE_DEFINITION_VISIBILITY PRIVATE)

if (NX_BUILD_TYPE STREQUAL "INTERFACE")
  set(NX_INCLUDE_VISIBILITY INTERFACE)
  set(NX_LINK_VISIBILITY INTERFACE)
  set(NX_COMPILE_DEFINITION_VISIBILITY INTERFACE)
endif()

target_include_directories(Nexus ${NX_INCLUDE_VISIBILITY} SYSTEM ${NEXUS_INCLUDE_DIRS})
target_link_libraries(Nexus ${NX_LINK_VISIBILITY} ${NEXUS_LINKED_LIBRARIES})
target_compile_definitions(Nexus ${NX_COMPILE_DEFINITION_VISIBILITY} ${NEXUS_COMPILE_DEFINITIONS})
nexus_structure_folders()
nexus_setup_filters("${ENGINE_SOURCES}")