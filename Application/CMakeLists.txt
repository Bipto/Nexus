cmake_minimum_required(VERSION 3.16)

project("Editor")

file (GLOB_RECURSE APP_HEADERS *.h)
file (GLOB_RECURSE APP_SOURCES *.cpp)
add_executable(Editor ${APP_HEADERS} ${APP_SOURCES})