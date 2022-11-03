cmake_minimum_required(VERSION 3.8)

project("NexusEngine")

#find_package(Python REQUIRED COMPONENTS Interpreter Development)

file (GLOB_RECURSE ENGINE_HEADERS NexusEngine/*.h)
file (GLOB_RECURSE ENGINE_SOURCES NexusEngine/*.cpp)
add_library(NexusEngine ${ENGINE_HEADERS} ${ENGINE_SOURCES})

file (GLOB_RECURSE APP_HEADERS Application/*.h)
file (GLOB_RECURSE APP_SOURCES Application/*.cpp)
add_executable(Editor ${APP_HEADERS} ${APP_SOURCES})

add_subdirectory(External/SDL)
add_subdirectory(External/Lua)
add_subdirectory(External/glad)

#NexusEngine
target_include_directories(NexusEngine PUBLIC External/SDL/include)
target_include_directories(NexusEngine PUBLIC External/SDL_mixer/include)
target_include_directories(NexusEngine PUBLIC External/Lua.lua-5.4.4)
target_include_directories(NexusEngine PUBLIC External/glad/include)
#target_include_directories(NexusEngine PUBLIC ${Python_INCLUDE_DIRS})

#target_link_directories(NexusEngine PUBLIC ${Python_LIBRARY_DIRS})
#target_link_libraries(NexusEngine PUBLIC ${Python_LIBRARIES})
target_link_libraries(NexusEngine PUBLIC lua_static)
target_link_libraries(NexusEngine PUBLIC SDL2main SDL2-static)
target_link_libraries(NexusEngine PUBLIC glad)

#Editor
target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)