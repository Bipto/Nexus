cmake_minimum_required(VERSION 3.16)

project("Client")

add_executable(Server server.cpp)

target_include_directories(Server PUBLIC NexusEngine)
target_link_libraries(Server PUBLIC NexusEngine)