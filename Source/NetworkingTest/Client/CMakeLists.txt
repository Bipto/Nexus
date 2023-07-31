cmake_minimum_required(VERSION 3.16)

project("Client")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Client client.cpp)

target_include_directories(Client PUBLIC NexusEngine)
target_link_libraries(Client PUBLIC NexusEngine)