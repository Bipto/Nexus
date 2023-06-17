cmake_minimum_required(VERSION 3.16)

project("NexusEngine")
set (CMAKE_CXX_STANDARD 17)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (EMSCRIPTEN)
    add_link_options("-lGL")
endif()

add_subdirectory(Editor)
add_subdirectory(Demo)
add_subdirectory(NexusEngine)

set(SHADERC_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
set(EFFCEE_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(EFFCEE_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(LIBNYQUIST_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(LUA_BUILD_BINARY OFF CACHE BOOL "" FORCE)
set(LUA_BUILD_COMPILER OFF CACHE BOOL "" FORCE)
set(RE2_BUILD_FRAMEWORK OFF CACHE BOOL "" FORCE)
set(RE2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(External/SDL)
add_subdirectory(External/Lua)
add_subdirectory(External/glad)
add_subdirectory(External/glm)
add_subdirectory(External/imgui)

if (NOT DEFINED EMSCRIPTEN)
add_subdirectory(External/glslang)
add_subdirectory(External/SPIRV-Cross)
add_subdirectory(External/SPIRV-Headers)
add_subdirectory(External/SPIRV-Tools)
add_subdirectory(External/shaderc)
add_subdirectory(External/tinyfiledialogs)
add_subdirectory(External/re2)
add_subdirectory(External/effcee)

add_definitions(-DLIBTYPE=SHARED)
add_subdirectory(External/OpenAL-Soft)
remove_definitions(-DLIBTYPE)

add_subdirectory(External/libnyquist)

endif()

set (BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(External/assimp)

#Editor
target_include_directories(Editor PUBLIC NexusEngine)
target_link_libraries(Editor PUBLIC NexusEngine)

target_include_directories(Demo PUBLIC NexusEngine)
target_link_libraries(Demo PUBLIC NexusEngine)

if (EMSCRIPTEN)
    set_target_properties(
        Editor
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1 -lGL"
    )
endif()