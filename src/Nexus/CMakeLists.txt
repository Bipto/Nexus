cmake_minimum_required(VERSION 3.28)

project ("Nexus")

include(FetchContent)
include(Utils.cmake)

if (NOT DEFINED NX_BUILD_TYPE OR EMSCRIPTEN)
  set(NX_BUILD_TYPE STATIC)
endif()

if (NX_BUILD_TYPE STREQUAL "SHARED")
  add_compile_definitions(NX_BUILD_DLL=1)
  add_compile_definitions(NX_EXPORT_API=1)
endif()

add_subdirectory(Audio)
add_subdirectory(Platform)
add_subdirectory(RHI)
add_subdirectory(Engine)