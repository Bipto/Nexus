cmake_minimum_required(VERSION 3.16)

project("Tests")


add_executable(Tests src/main.cpp)
add_subdirectory(external/googletest)

target_include_directories(Tests PUBLIC external/googletest/googletest/include)
target_link_libraries(Tests PUBLIC gtest gtest_main)

add_test(
    NAME Tests
    COMMAND Tests
)