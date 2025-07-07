cmake_minimum_required(VERSION 3.8)

project ("glad2")

add_library(glad2 INTERFACE)
target_include_directories(glad2 INTERFACE include src)