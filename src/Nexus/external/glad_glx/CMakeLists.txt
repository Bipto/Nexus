cmake_minimum_required(VERSION 3.8)

project("glad_glx")

add_library(glad_glx include/glad/glad_glx.h include/KHR/khrplatform.h src/glad_glx.c)
target_include_directories(glad_glx PUBLIC include/ ../glad/include)