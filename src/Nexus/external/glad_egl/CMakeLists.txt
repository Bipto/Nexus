cmake_minimum_required(VERSION 3.8)

project("glad_egl")

add_library(glad_egl include/EGL/eglplatform.h include/glad/glad_egl.h include/KHR/khrplatform.h src/glad_egl.c)
target_include_directories(glad_egl PUBLIC include/ ../glad/include)

if (ANDROID)
    find_library(GLES3_LIB GLESv3)
    target_link_libraries(glad_egl PUBLIC EGL GLESv2 GLESv3)
endif()