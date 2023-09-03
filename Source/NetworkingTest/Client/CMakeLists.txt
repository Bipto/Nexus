cmake_minimum_required(VERSION 3.16)

project("Client")

add_executable(Client client.cpp)

target_include_directories(Client PUBLIC NexusEngine)
target_link_libraries(Client PUBLIC NexusEngine)