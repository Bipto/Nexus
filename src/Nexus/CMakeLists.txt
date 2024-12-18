cmake_minimum_required(VERSION 3.16)

project ("Nexus")

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)

add_compile_definitions(NX_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
add_compile_definitions(NX_BUILD_TYPE=${CMAKE_BUILD_TYPE})

if (DEFINED NX_PLATFORM_OPENGL)
    find_package(OpenGL REQUIRED)
    add_compile_definitions(NX_PLATFORM_OPENGL)
endif()

if (DEFINED NX_PLATFORM_D3D12)
    add_compile_definitions(NX_PLATFORM_D3D12)
endif()

if (DEFINED NX_PLATFORM_VULKAN)
    find_package(Vulkan REQUIRED)
    add_compile_definitions(NX_PLATFORM_VULKAN)
endif()

if (DEFINED NX_PLATFORM_WGL)
    add_compile_definitions(NX_PLATFORM_WGL)
endif()

if (DEFINED NX_PLATFORM_EGL)
    add_compile_definitions(NX_PLATFORM_EGL)
endif()

if (DEFINED NX_PLATFORM_WEBGL)
    add_compile_definitions(NX_PLATFORM_WEBGL)
endif()

if (DEFINED NX_PLATFORM_GL_GLAD)
    add_compile_definitions(NX_PLATFORM_GL_GLAD)
endif()

if (DEFINED NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_OPENAL)
endif()

#platform specific configurations
if (WIN32)
    add_compile_definitions(NX_PLATFORM_WINDOWS)
    add_compile_definitions(NX_PLATFORM_GL_DESKTOP)
elseif(LINUX)
    add_compile_definitions(NX_PLATFORM_LINUX)
elseif(EMSCRIPTEN)
    add_compile_definitions(NX_PLATFORM_WEB)
endif()

if (ANDROID)
    add_compile_definitions(NX_PLATFORM_OPENGL)
    add_compile_definitions(NX_PLATFORM_OPENAL)
    add_compile_definitions(NX_PLATFORM_GL_GLAD)
    add_compile_definitions(NX_PLATFORM_EGL)
endif()

#external dependencies
add_subdirectory(external/SDL)
add_subdirectory(external/glad)

if (WIN32)
  add_subdirectory(external/glad_wgl)
endif()

if(ANDROID OR __ANDROID__ OR LINUX)
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
  src/Platform/Standard/FileSystemStandard.cpp)

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

if (WIN32)
  add_library(Nexus ${ENGINE_SOURCES})
else()
  add_library(Nexus ${ENGINE_SOURCES})
endif()

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
        "--std=c++20 -O3 -flto -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -sNO_DISABLE_EXCEPTION_CATCHING -lopenal -lembind -sGL_ENABLE_GET_PROC_ADDRESS"
    )
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

if (ANDROID OR __ANDROID__ OR LINUX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(EGL REQUIRED egl)

  target_link_libraries(Nexus PUBLIC glad_egl)
  target_include_directories(Nexus PUBLIC ${EGL_INCLUDE_DIRS})
  target_link_libraries(Nexus PUBLIC ${EGL_LIBRARIES})
endif()

if (LINUX)
  find_package(X11 REQUIRED)
  target_link_libraries(Nexus PUBLIC ${X11_LIBRARIES})
  include_directories(${X11_INCLUDE_DIR})
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