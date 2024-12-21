cmake_minimum_required(VERSION 3.16)

add_subdirectory(cmake)
project ("Nexus")

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

add_compile_definitions(NX_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
add_compile_definitions(NX_BUILD_TYPE=${CMAKE_BUILD_TYPE})
add_compile_definitions(NX_SYSTEM_PROCESSOR=${CMAKE_HOST_SYSTEM_PROCESSOR})

set(NEXUS_INCLUDE_DIRS "")
set(NEXUS_LINKED_LIBRARIES "")

if (NOT EMSCRIPTEN)
  list (APPEND NEXUS_INCLUDE_DIRS external/tinyfiledialogs/tinyfiledialogs)
  list (APPEND NEXUS_LINKED_LIBRARIES tinyfiledialogs)
endif()

if (DEFINED NX_PLATFORM_OPENGL)
  find_package(OpenGL REQUIRED)
  add_compile_definitions(NX_PLATFORM_OPENGL)

  if (WIN32)
      add_compile_definitions(NX_PLATFORM_WGL)
      add_compile_definitions(NX_PLATFORM_GL_GLAD)
      add_compile_definitions(NX_PLATFORM_GL_DESKTOP)
      add_subdirectory(external/glad_wgl)
      list(APPEND NEXUS_LINKED_LIBRARIES glad_wgl)
  elseif(LINUX OR ANDROID OR __ANDROID__)
    if (LINUX)
      add_compile_definitions(NX_PLATFORM_GL_DESKTOP)
    endif()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EGL REQUIRED egl)
    add_compile_definitions(NX_PLATFORM_EGL)
    add_compile_definitions(NX_PLATFORM_GL_GLAD)
    list (APPEND NEXUS_INCLUDE_DIRS ${EGL_INCLUDE_DIRS})
    list(APPEND NEXUS_LINKED_LIBRARIES glad_egl ${EGL_LIBRARIES})
    add_subdirectory(external/glad_egl)
  elseif (EMSCRIPTEN)
      add_compile_definitions(NX_PLATFORM_WEBGL)
  endif()

endif()

if (DEFINED NX_PLATFORM_D3D12)
    add_compile_definitions(NX_PLATFORM_D3D12)
endif()

if (DEFINED NX_PLATFORM_VULKAN)
  add_compile_definitions(NX_PLATFORM_VULKAN)
  find_package(Vulkan REQUIRED)
  add_subdirectory(external/VulkanMemoryAllocator)
  set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "external")
  set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)

  list(APPEND NEXUS_LINKED_LIBRARIES Vulkan::Vulkan VulkanMemoryAllocator)
  list (APPEND NEXUS_INCLUDE_DIRS ${VULKAN_INCLUDE_DIRS} external/VulkanMemoryAllocator/include)
endif()

if (DEFINED NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_OPENAL)

    if (NOT EMSCRIPTEN)
      nexus_add_shared_library("external/OpenAL-Soft")
      list (APPEND NEXUS_LINKED_LIBRARIES OpenAL)
    endif()
    
endif()

#platform specific configurations
if (WIN32)
    add_compile_definitions(NX_PLATFORM_WINDOWS)
elseif(LINUX)
    add_compile_definitions(NX_PLATFORM_LINUX)
elseif(EMSCRIPTEN)
    add_compile_definitions(NX_PLATFORM_WEB)
elseif(ANDROID)

endif()

#external dependencies
add_subdirectory(external/SDL)
add_subdirectory(external/glad)
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
nexus_add_shared_library("external/assimp")

#these libraries are only required if we are not building for emscripten
#this is because these libraries are incompatible with emscripten
if (WIN32 OR LINUX)
    add_subdirectory(external/tinyfiledialogs)
    set_target_properties(tinyfiledialogs PROPERTIES FOLDER "external")
endif()

set (ENGINE_SOURCES
  src/Nexus-Core/Application.cpp
  src/Nexus-Core/Application.hpp
  src/Nexus-Core/ApplicationSpecification.hpp
  src/Nexus-Core/AssetManager.cpp
  src/Nexus-Core/AssetManager.hpp
  src/Nexus-Core/Buffer.hpp
  src/Nexus-Core/EntryPoint.hpp
  src/Nexus-Core/IView.hpp
  src/Nexus-Core/Monitor.hpp
  src/Nexus-Core/nxpch.cpp
  src/Nexus-Core/nxpch.hpp
  src/Nexus-Core/Platform.cpp
  src/Nexus-Core/Platform.hpp
  src/Nexus-Core/Point.hpp
  src/Nexus-Core/Renderer.cpp
  src/Nexus-Core/Renderer.hpp
  src/Nexus-Core/Runtime.cpp
  src/Nexus-Core/Runtime.hpp
  src/Nexus-Core/Types.hpp
  src/Nexus-Core/Vertex.cpp
  src/Nexus-Core/Vertex.hpp
  src/Nexus-Core/Window.cpp
  src/Nexus-Core/Window.hpp
  src/Nexus-Core/WindowInfo.hpp
)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Audio/AudioBuffer.hpp
  src/Nexus-Core/Audio/AudioDevice.hpp
  src/Nexus-Core/Audio/AudioSource.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Events/EventHandler.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/FileSystem/FileDialogs.cpp
  src/Nexus-Core/FileSystem/FileDialogs.hpp
  src/Nexus-Core/FileSystem/FileSystem.cpp
  src/Nexus-Core/FileSystem/FileSystem.hpp
  src/Nexus-Core/FileSystem/FileSystemNew.hpp)

list (APPEND ENGINE_SOURCES 
  src/Nexus-Core/Graphics/CachedShader.hpp
  src/Nexus-Core/Graphics/CachedTexture.hpp
  src/Nexus-Core/Graphics/CatmullRom.hpp
  src/Nexus-Core/Graphics/Circle.hpp
  src/Nexus-Core/Graphics/Color.hpp
  src/Nexus-Core/Graphics/CommandExecutor.cpp
  src/Nexus-Core/Graphics/CommandExecutor.hpp
  src/Nexus-Core/Graphics/CommandList.cpp
  src/Nexus-Core/Graphics/CommandList.hpp
  src/Nexus-Core/Graphics/Font.cpp
  src/Nexus-Core/Graphics/Font.hpp
  src/Nexus-Core/Graphics/Framebuffer.hpp
  src/Nexus-Core/Graphics/FullscreenQuad.cpp
  src/Nexus-Core/Graphics/FullscreenQuad.hpp
  src/Nexus-Core/Graphics/GPUBuffer.hpp
  src/Nexus-Core/Graphics/GraphicsCapabilities.hpp
  src/Nexus-Core/Graphics/GraphicsDevice.cpp
  src/Nexus-Core/Graphics/GraphicsDevice.hpp
  src/Nexus-Core/Graphics/HdriProcessor.cpp
  src/Nexus-Core/Graphics/HdriProcessor.hpp
  src/Nexus-Core/Graphics/IResource.hpp
  src/Nexus-Core/Graphics/Mesh.hpp
  src/Nexus-Core/Graphics/MeshFactory.cpp
  src/Nexus-Core/Graphics/MeshFactory.hpp
  src/Nexus-Core/Graphics/MipmapGenerator.cpp
  src/Nexus-Core/Graphics/MipmapGenerator.hpp
  src/Nexus-Core/Graphics/Model.hpp
  src/Nexus-Core/Graphics/Multisample.cpp
  src/Nexus-Core/Graphics/Multisample.hpp
  src/Nexus-Core/Graphics/Pipeline.hpp
  src/Nexus-Core/Graphics/PixelFormat.hpp
  src/Nexus-Core/Graphics/Polygon.hpp
  src/Nexus-Core/Graphics/Rectangle.hpp
  src/Nexus-Core/Graphics/RenderTarget.hpp
  src/Nexus-Core/Graphics/ResourceSet.cpp
  src/Nexus-Core/Graphics/ResourceSet.hpp
  src/Nexus-Core/Graphics/RoundedRectangle.hpp
  src/Nexus-Core/Graphics/Sampler.hpp
  src/Nexus-Core/Graphics/SamplerState.hpp
  src/Nexus-Core/Graphics/Scissor.hpp
  src/Nexus-Core/Graphics/ShaderDataType.cpp
  src/Nexus-Core/Graphics/ShaderDataType.hpp
  src/Nexus-Core/Graphics/ShaderGenerator.cpp
  src/Nexus-Core/Graphics/ShaderGenerator.hpp
  src/Nexus-Core/Graphics/ShaderModule.hpp
  src/Nexus-Core/Graphics/ShaderUtils.hpp
  src/Nexus-Core/Graphics/stb.cpp
  src/Nexus-Core/Graphics/Structures.hpp
  src/Nexus-Core/Graphics/Swapchain.hpp
  src/Nexus-Core/Graphics/Texture.cpp
  src/Nexus-Core/Graphics/Texture.hpp
  src/Nexus-Core/Graphics/TimingQuery.hpp
  src/Nexus-Core/Graphics/Triangle.hpp
  src/Nexus-Core/Graphics/Viewport.hpp
  src/Nexus-Core/Graphics/WindingOrder.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/ImGui/ImGuiGraphicsRenderer.cpp
  src/Nexus-Core/ImGui/ImGuiGraphicsRenderer.hpp
  src/Nexus-Core/ImGui/ImGuiInclude.hpp  
)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Input/Event.hpp
  src/Nexus-Core/Input/Gamepad.cpp
  src/Nexus-Core/Input/Gamepad.hpp
  src/Nexus-Core/Input/Input.cpp
  src/Nexus-Core/Input/Input.hpp
  src/Nexus-Core/Input/InputContext.cpp
  src/Nexus-Core/Input/InputContext.hpp
  src/Nexus-Core/Input/InputState.cpp
  src/Nexus-Core/Input/InputState.hpp
  src/Nexus-Core/Input/Keyboard.cpp
  src/Nexus-Core/Input/Keyboard.hpp
  src/Nexus-Core/Input/Mouse.cpp
  src/Nexus-Core/Input/Mouse.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Layers/Layer.hpp
  src/Nexus-Core/Layers/LayerStack.cpp
  src/Nexus-Core/Layers/LayerStack.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Logging/Log.cpp
  src/Nexus-Core/Logging/Log.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Renderer/BatchRenderer.cpp
  src/Nexus-Core/Renderer/BatchRenderer.hpp
)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Runtime/Camera.hpp
  src/Nexus-Core/Runtime/Entity.cpp
  src/Nexus-Core/Runtime/Entity.hpp
  src/Nexus-Core/Runtime/Project.cpp
  src/Nexus-Core/Runtime/Project.hpp
  src/Nexus-Core/Runtime/Scene.cpp
  src/Nexus-Core/Runtime/Scene.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Timings/Clock.hpp
  src/Nexus-Core/Timings/Profiler.cpp
  src/Nexus-Core/Timings/Profiler.hpp
  src/Nexus-Core/Timings/Timer.hpp
  src/Nexus-Core/Timings/Timespan.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/UI/Canvas.hpp)

list (APPEND ENGINE_SOURCES
  src/Nexus-Core/Utils/FramerateMonitor.hpp
  src/Nexus-Core/Utils/GUID.hpp
  src/Nexus-Core/Utils/Utils.cpp
  src/Nexus-Core/Utils/Utils.hpp)

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

list (APPEND ENGINE_SOURCES
  src/Platform/OpenAL/AudioBufferOpenAL.cpp
  src/Platform/OpenAL/AudioBufferOpenAL.hpp
  src/Platform/OpenAL/AudioDeviceOpenAL.cpp
  src/Platform/OpenAL/AudioDeviceOpenAL.hpp
  src/Platform/OpenAL/AudioSourceOpenAL.cpp
  src/Platform/OpenAL/AudioSourceOpenAL.hpp
  src/Platform/OpenAL/OpenAL.hpp)

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

list (APPEND ENGINE_SOURCES
  src/Platform/OpenGL/Context/IOffscreenContext.hpp
  src/Platform/OpenGL/Context/IViewContext.hpp)

list (APPEND ENGINE_SOURCES
  src/Platform/OpenGL/Context/EGL/EGLUtils.hpp
  src/Platform/OpenGL/Context/EGL/OffscreenContextEGL.cpp
  src/Platform/OpenGL/Context/EGL/OffscreenContextEGL.hpp
  src/Platform/OpenGL/Context/EGL/ViewContextEGL.cpp
  src/Platform/OpenGL/Context/EGL/ViewContextEGL.hpp)

list (APPEND ENGINE_SOURCES
  src/Platform/OpenGL/Context/WebGL/OffscreenContextWebGL.cpp
  src/Platform/OpenGL/Context/WebGL/OffscreenContextWebGL.hpp
  src/Platform/OpenGL/Context/WebGL/ViewContextWebGL.cpp
  src/Platform/OpenGL/Context/WebGL/ViewContextWebGL.hpp)

list (APPEND ENGINE_SOURCES
  src/Platform/OpenGL/Context/WGL/OffscreenContextWGL.cpp
  src/Platform/OpenGL/Context/WGL/OffscreenContextWGL.hpp
  src/Platform/OpenGL/Context/WGL/ViewContextWGL.cpp
  src/Platform/OpenGL/Context/WGL/ViewContextWGL.hpp
  )

list (APPEND ENGINE_SOURCES
  src/Platform/SDL3/SDL3Include.cpp
  src/Platform/SDL3/SDL3Include.hpp
  src/Platform/SDL3/SDL3Window.cpp
  src/Platform/SDL3/SDL3Window.hpp
  src/Platform/SDL3/SDLPlatform.cpp)

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

list (APPEND ENGINE_SOURCES
  src/Platform/Windows/FileSystemWindows.cpp
  src/Platform/Windows/WindowsInclude.hpp)

list (APPEND ENGINE_SOURCES
  src/Platform/X11/X11Include.hpp)

add_library(Nexus ${ENGINE_SOURCES})
nexus_setup_filters(${ENGINE_SOURCES})

#including dependencies
list (APPEND NEXUS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)
list (APPEND NEXUS_INCLUDE_DIRS external/SDL/include/SDL3)
list (APPEND NEXUS_INCLUDE_DIRS external/glm)
list (APPEND NEXUS_INCLUDE_DIRS external/stb)
list (APPEND NEXUS_INCLUDE_DIRS external/assimip/include)
list (APPEND NEXUS_INCLUDE_DIRS external/json/single_include)
list (APPEND NEXUS_INCLUDE_DIRS external/OpenAL-Soft/include)
list (APPEND NEXUS_INCLUDE_DIRS external/shaderc/libshaderc)
list (APPEND NEXUS_INCLUDE_DIRS external/libnyquist/include)
list (APPEND NEXUS_INCLUDE_DIRS external/SPIRV-Cross/include)
list (APPEND NEXUS_INCLUDE_DIRS external/SPIRV-Reflect)
list (APPEND NEXUS_INCLUDE_DIRS external/Freetype/include)
list (APPEND NEXUS_INCLUDE_DIRS external/pocketpy/include)
list (APPEND NEXUS_INCLUDE_DIRS external/yaml-cpp/include)
list (APPEND NEXUS_INCLUDE_DIRS binaries/dxc_2023_08_14/inc)
target_include_directories(Nexus PUBLIC ${NEXUS_INCLUDE_DIRS})

list (APPEND NEXUS_LINKED_LIBRARIES ${OPENGL_LIBRARIES})
list (APPEND NEXUS_LINKED_LIBRARIES SDL3::SDL3)
list (APPEND NEXUS_LINKED_LIBRARIES glad)
list (APPEND NEXUS_LINKED_LIBRARIES assimp)
list (APPEND NEXUS_LINKED_LIBRARIES imgui)
list (APPEND NEXUS_LINKED_LIBRARIES imguizmo)
list (APPEND NEXUS_LINKED_LIBRARIES shaderc)
list (APPEND NEXUS_LINKED_LIBRARIES libnyquist)
list (APPEND NEXUS_LINKED_LIBRARIES spirv-cross-cpp)
list (APPEND NEXUS_LINKED_LIBRARIES spirv-cross-hlsl)
list (APPEND NEXUS_LINKED_LIBRARIES pocketpy)
list (APPEND NEXUS_LINKED_LIBRARIES freetype)
list (APPEND NEXUS_LINKED_LIBRARIES yaml-cpp::yaml-cpp)
target_link_libraries(Nexus PUBLIC ${NEXUS_LINKED_LIBRARIES})

#precompiled header
target_precompile_headers(Nexus PUBLIC src/Nexus-Core/nxpch.hpp)

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Nexus
        PROPERTIES
        LINK_FLAGS
        "--std=c++20 -O3 -flto -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS"
    )
endif()

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

nexus_structure_folders()