cmake_minimum_required(VERSION 3.8)

project ("glad2")

set (GLAD_SOURCES "")
set (GLAD_INCLUDES "include")

if (DEFINED WIN32)
    list (APPEND GLAD_SOURCES "include/glad/wgl.h" "src/wgl.c")
endif()

if (DEFINED NX_PLATFORM_WEBGL OR DEFINED NX_PLATFORM_OPENGLES)
    list (APPEND GLAD_SOURCES "include/glad/gles2.h" "src/gles2.c")
else()
    list (APPEND GLAD_SOURCES "include/glad/gl.h" "src/gl.c")
endif()

if (GLAD_SOURCES)
    add_library(glad2 ${GLAD_SOURCES})
    target_include_directories(glad2 PUBLIC include/)
else()
    add_library(glad2 INTERFACE)
    target_include_directories(glad2 INTERFACE include/)
endif()