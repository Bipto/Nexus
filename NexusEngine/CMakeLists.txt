cmake_minimum_required(VERSION 3.16)

project ("NexusEngine")

#find_package(Python REQUIRED COMPONENTS Interpreter Development)

file (GLOB_RECURSE ENGINE_HEADERS Core/*.h Platform/*.h Runtime/*.h Runtime/ECS/*.h)
file (GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Platform/*.cpp Platform/OpenGL*.cpp Platform/DX11/*.cpp Runtime/*.cpp Runtime/ECS/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

target_include_directories(NexusEngine PUBLIC ../External/SDL/include)
target_include_directories(NexusEngine PUBLIC ../External/SDL_mixer/include)
target_include_directories(NexusEngine PRIVATE External/Lua.lua-5.4.4)
#target_include_directories(NexusEngine PUBLIC ${Python_INCLUDE_DIRS})
target_include_directories(NexusEngine PUBLIC ../External/glm)
target_include_directories(NexusEngine PUBLIC ../External/stb)
target_include_directories(NexusEngine PUBLIC ../External/assimp/include)
target_include_directories(NexusEngine PUBLIC ../External/json/single_include)
target_include_directories(NexusEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NexusEngine PUBLIC SDL2main SDL2-static)
target_link_libraries(NexusEngine PUBLIC lua_static)
#target_link_directories(NexusEngine PUBLIC ${Python_LIBRARY_DIRS})
#target_link_libraries(NexusEngine PUBLIC ${Python_LIBRARIES})
target_link_libraries(NexusEngine PUBLIC glad)
target_link_libraries(NexusEngine PUBLIC assimp)
target_link_libraries(NexusEngine PUBLIC imgui)
target_link_libraries(NexusEngine PUBLIC imguizmo)

find_package(OpenGL REQUIRED)
target_link_libraries(NexusEngine PUBLIC ${OPENGL_LIBRARIES})

target_precompile_headers(NexusEngine PUBLIC Core/nxpch.h)