cmake_minimum_required(VERSION 3.16)

project ("NexusEngine")
set (CMAKE_CXX_STANDARD 17)

#google test - required for shaderc
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#adds cpp and h files from engine
file (GLOB_RECURSE ENGINE_HEADERS Core/*.h Platform/*.h Runtime/*.h Runtime/ECS/*.h)
file (GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Platform/*.cpp Platform/OpenGL*.cpp Platform/DX11/*.cpp Runtime/*.cpp Runtime/ECS/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#including dependencies
target_include_directories(NexusEngine PUBLIC ../External/SDL/include)
target_include_directories(NexusEngine PUBLIC ../External/SDL_mixer/include)
target_include_directories(NexusEngine PRIVATE External/Lua.lua-5.4.4)
target_include_directories(NexusEngine PUBLIC ../External/glm)
target_include_directories(NexusEngine PUBLIC ../External/stb)
target_include_directories(NexusEngine PUBLIC ../External/assimp/include)
target_include_directories(NexusEngine PUBLIC ../External/json/single_include)
target_include_directories(NexusEngine PUBLIC ../External/shaderc/libshaderc)
target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#linking dependencies
target_link_libraries(NexusEngine PUBLIC SDL2main SDL2-static)
target_link_libraries(NexusEngine PUBLIC lua_static)
target_link_libraries(NexusEngine PUBLIC glad)
target_link_libraries(NexusEngine PUBLIC assimp)
target_link_libraries(NexusEngine PUBLIC imgui)
target_link_libraries(NexusEngine PUBLIC imguizmo)
target_link_libraries(NexusEngine PUBLIC shaderc)

#including OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(NexusEngine PUBLIC ${OPENGL_LIBRARIES})

#precompiled header
target_precompile_headers(NexusEngine PUBLIC Core/nxpch.h)