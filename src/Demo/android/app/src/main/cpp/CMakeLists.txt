cmake_minimum_required(VERSION 3.30)

project("NexusDemoAndroid")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

add_subdirectory(C:\\Dev\\Nexus\\src\\Nexus Nexus)

find_library(log-lib
        log)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(helloworld-c SHARED
        helloworld-c.cpp)

target_link_libraries(helloworld-c
        android
        native_app_glue
        ${log-lib})

target_link_libraries(helloworld-c
    Nexus
)