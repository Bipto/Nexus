cmake_minimum_required(VERSION 3.16)

project("Demo")

if (ANDROID OR __ANDROID__)
    add_library(Demo SHARED main.cpp)
else()
    add_executable(Demo main.cpp)
endif()

target_include_directories(Demo PUBLIC NexusEngine)
target_link_libraries(Demo PUBLIC NexusEngine)

#resources need to be output into a separate folder if building for emscripten
if (NOT EMSCRIPTEN)
   set(path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   cmake_path(GET path PARENT_PATH parentDir)
   set_target_properties(Demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${parentDir}/bin)
   file (COPY
       ${CMAKE_CURRENT_SOURCE_DIR}/resources
       DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

#flags required for emscripten
if (EMSCRIPTEN)
    set_target_properties(
      Demo
        PROPERTIES
        LINK_FLAGS
        "--std=c++17 -Os -sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES2=1 -sFULL_ES3=1 -sEMULATE_FUNCTION_POINTER_CASTS -lopenal -sGL_ENABLE_GET_PROC_ADDRESS=1"
    )

    set_target_properties(NexusEngine PROPERTIES COMPILE_FLAGS "-Os")
endif()

if (ANDROID OR __ANDROID__)
    target_link_libraries(
        Demo
        PUBLIC
        EGL
        GLESv2
        GLESv3
    )
endif()