cmake_minimum_required(VERSION 3.16)

project("Nexus")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

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

#set up engine projects
add_subdirectory(src/BuildConfig)
add_subdirectory(src/Nexus)
add_subdirectory(src/Demo)

enable_testing()
add_subdirectory(src/Tests)

if(WIN32 OR LINUX)
    add_subdirectory(src/Sandbox)
endif()