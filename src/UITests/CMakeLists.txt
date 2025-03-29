cmake_minimum_required(VERSION 3.30)

project("UITests")

add_executable(UITests main.cpp)
target_link_libraries(UITests PUBLIC Nexus)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()