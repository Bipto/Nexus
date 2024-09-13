cmake_minimum_required(VERSION 3.8)

project ("glad_wgl")

add_library(glad_wgl include/glad/glad_wgl.h include/KHR/khrplatform.h src/glad_wgl.c)
target_include_directories(glad_wgl PUBLIC include/ ../glad/include)