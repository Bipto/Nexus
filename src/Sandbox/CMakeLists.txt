cmake_minimum_required(VERSION 3.30)

project("Sandbox")

add_executable(Sandbox main.cpp)
target_link_libraries(Sandbox PUBLIC Nexus)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()