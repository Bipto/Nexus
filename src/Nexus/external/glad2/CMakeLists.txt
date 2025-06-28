cmake_minimum_required(VERSION 3.8)

project ("glad2")

set (GLAD_SOURCES "")
set (GLAD_INCLUDES "")

if (GLAD_SOURCES)
    add_library(glad2 ${GLAD_SOURCES})
    target_include_directories(glad2 PUBLIC include/)
else()
    add_library(glad2 INTERFACE)
    target_include_directories(glad2 INTERFACE include/)
endif()