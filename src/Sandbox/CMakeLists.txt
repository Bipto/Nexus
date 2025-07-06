cmake_minimum_required(VERSION 3.28)

project("Sandbox")

add_executable(Sandbox main.cpp)
target_link_libraries(Sandbox PUBLIC Nexus)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EGL REQUIRED egl)

include_directories(${EGL_INCLUDE_DIRS})
link_directories(${EGL_LIBRARY_DIRS})
add_definitions(${EGL_CFLAGS_OTHER})
target_link_libraries(Sandbox PRIVATE ${EGL_LIBRARIES})

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()