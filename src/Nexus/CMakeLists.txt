cmake_minimum_required(VERSION 3.16)

add_subdirectory(cmake)
project ("Nexus")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS=1)
add_definitions(-DNX_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
add_definitions(-DNX_SYSTEM_PROCESSOR=${CMAKE_HOST_SYSTEM_PROCESSOR})

set(NEXUS_INCLUDE_DIRS "")
set(NEXUS_LINKED_LIBRARIES "")
set(NEXUS_LIBRARY_SUBDIRECTORIES "")
set(NEXUS_SHARED_LIBRARY_SUBDIRECTORIES "")

#platform specific configurations
if (WIN32)
    add_definitions(-DNX_PLATFORM_WINDOWS=1)
    set(NX_PLATFORM_WINDOWS ON CACHE BOOL "")
elseif(LINUX)
  add_definitions(-DNX_PLATFORM_LINUX=1)
elseif(EMSCRIPTEN)
  add_definitions(-DNX_PLATFORM_WEB=1)
    set(NX_PLATFORM_WEB ON CACHE BOOL "")
elseif(ANDROID)

endif()

if (NOT EMSCRIPTEN)
  list (APPEND NEXUS_INCLUDE_DIRS external/tinyfiledialogs/tinyfiledialogs)
  list (APPEND NEXUS_LINKED_LIBRARIES tinyfiledialogs)
endif()

if (DEFINED NX_PLATFORM_OPENGL)
  find_package(OpenGL REQUIRED)
  add_definitions(-DNX_PLATFORM_OPENGL=1)

  if (WIN32)
      add_definitions(-DNX_PLATFORM_WGL=1)
      add_definitions(-DNX_PLATFORM_GL_GLAD=1)
      add_definitions(-DNX_PLATFORM_GL_DESKTOP=1)
      list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad_wgl)
      list(APPEND NEXUS_LINKED_LIBRARIES glad_wgl)
      set(NX_PLATFORM_WGL ON CACHE BOOL "")

  elseif(LINUX OR ANDROID OR __ANDROID__)
    if (LINUX)
      add_definitions(-DNX_PLATFORM_GL_DESKTOP=1)
    endif()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EGL REQUIRED egl)
    add_definitions(-DNX_PLATFORM_EGL=1)
    add_definitions(-DNX_PLATFORM_GL_GLAD=1)
    list (APPEND NEXUS_INCLUDE_DIRS ${EGL_INCLUDE_DIRS})
    list(APPEND NEXUS_LINKED_LIBRARIES glad_egl ${EGL_LIBRARIES})
    list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/glad_egl)
    set(NX_PLATFORM_EGL ON CACHE BOOL "")

  elseif (EMSCRIPTEN)
      add_definitions(-DNX_PLATFORM_WEBGL=1)
      set(NX_PLATFORM_WEBGL ON CACHE BOOL "")
  endif()

endif()

if (DEFINED NX_PLATFORM_D3D12)
    add_definitions(-DNX_PLATFORM_D3D12=1)
endif()

if (DEFINED NX_PLATFORM_VULKAN)
  add_definitions(-DNX_PLATFORM_VULKAN=1)
  find_package(Vulkan REQUIRED)
  list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/VulkanMemoryAllocator)
  list(APPEND NEXUS_LINKED_LIBRARIES Vulkan::Vulkan VulkanMemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${VULKAN_INCLUDE_DIRS} external/VulkanMemoryAllocator/include)
endif()

if (DEFINED NX_PLATFORM_OPENAL)
    add_definitions(-DNX_PLATFORM_OPENAL=1)

    if (NOT EMSCRIPTEN)
      list (APPEND NEXUS_SHARED_LIBRARY_SUBDIRECTORIES external/OpenAL-Soft)
      list (APPEND NEXUS_LINKED_LIBRARIES OpenAL)
    endif()
    
endif()

#external dependencies
list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES 
  external/SDL
  external/glad
  external/glm
  external/imgui
  external/glslang
  external/SPIRV-Cross
  external/SPIRV-Headers
  external/SPIRV-Tools
  external/shaderc
  external/libnyquist
  external/freetype
  external/pocketpy
  external/yaml-cpp)
  
list(APPEND NEXUS_SHARED_LIBRARY_SUBDIRECTORIES external/assimp)

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (WIN32 OR LINUX)
  list(APPEND NEXUS_LIBRARY_SUBDIRECTORIES external/tinyfiledialogs)
endif()

set (ENGINE_SOURCES
  src/Nexus-Core/Application.cpp
  include/Nexus-Core/Application.hpp
  include/Nexus-Core/ApplicationSpecification.hpp
  src/Nexus-Core/AssetManager.cpp
  include/Nexus-Core/AssetManager.hpp
  include/Nexus-Core/EntryPoint.hpp
  include/Nexus-Core/IView.hpp
  include/Nexus-Core/Monitor.hpp
  src/Nexus-Core/nxpch.cpp
  include/Nexus-Core/nxpch.hpp
  src/Nexus-Core/Platform.cpp
  include/Nexus-Core/Platform.hpp
  include/Nexus-Core/Point.hpp
  src/Nexus-Core/Renderer.cpp
  include/Nexus-Core/Renderer.hpp
  src/Nexus-Core/Runtime.cpp
  include/Nexus-Core/Runtime.hpp
  include/Nexus-Core/Types.hpp
  src/Nexus-Core/Vertex.cpp
  include/Nexus-Core/Vertex.hpp
  src/Nexus-Core/Window.cpp
  include/Nexus-Core/Window.hpp
  include/Nexus-Core/WindowInfo.hpp
)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Audio/AudioBuffer.hpp
  include/Nexus-Core/Audio/AudioDevice.hpp
  include/Nexus-Core/Audio/AudioSource.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/ECS/ComponentRegistry.hpp
  src/Nexus-Core/ECS/Components.cpp
  include/Nexus-Core/ECS/Registry.hpp)

list (APPEND ENGINE_SOURCES
  include/Nexus-Core/Events/EventHandler.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/FileSystem/FileDialogs.cpp
  include/Nexus-Core/FileSystem/FileDialogs.hpp
  src/Nexus-Core/FileSystem/FileSystem.cpp
  include/Nexus-Core/FileSystem/FileSystem.hpp)

list (APPEND ENGINE_SOURCES 
  include/Nexus-Core/Graphics/CachedShader.hpp
  include/Nexus-Core/Graphics/CachedTexture.hpp
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
  src/Nexus-Core/Input/Gamepad.cpp
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
      add_definitions(-DVK_USE_PLATFORM_WIN32_KHR=1)
    endif()

    if (LINUX)
      list (APPEND ENGINE_SOURCES
        src/Platform/Vulkan/Xlib/PlatformVkXlib.cpp)
        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR=1)
    endif()
endif()

if (DEFINED NX_PLATFORM_WINDOWS)
  list (APPEND ENGINE_SOURCES
    src/Platform/Windows/WindowsInclude.hpp)
endif()

if (DEFINED NX_PLATFORM_LINUX)
  list (APPEND ENGINE_SOURCES src/Platform/X11/X11Include.hpp)
endif()

add_library(Nexus ${ENGINE_SOURCES})
nexus_setup_filters(${ENGINE_SOURCES})

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
  external/SDL/include
  external/glm
  external/stb
  external/assimip/include
  external/json/single_include
  external/imgui/imgui
  external/imgui/imguizmo
  external/OpenAL-Soft/include
  external/shaderc/libshaderc
  external/libnyquist/include
  external/SPIRV-Cross/include
  external/SPIRV-Reflect
  external/Freetype/include
  external/pocketpy/include
  external/yaml-cpp/include
  binaries/dxc_2023_08_14/inc)

list (APPEND NEXUS_LINKED_LIBRARIES 
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
  yaml-cpp::yaml-cpp)

#precompiled header
target_precompile_headers(Nexus PUBLIC include/Nexus-Core/nxpch.hpp)

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
target_include_directories(Nexus PUBLIC SYSTEM ${NEXUS_INCLUDE_DIRS})
target_link_libraries(Nexus PRIVATE ${NEXUS_LINKED_LIBRARIES})
target_include_directories(Nexus PUBLIC include)
nexus_structure_folders()
nexus_setup_filters("${ENGINE_SOURCES}")

if (DEFINED NX_PLATFORM_VULKAN)
  set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20)
endif()