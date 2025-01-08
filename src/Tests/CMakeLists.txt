cmake_minimum_required(VERSION 3.16)

project("Tests")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

FetchContent_MakeAvailable(googletest)

add_executable(Tests src/main.cpp)

target_include_directories(Tests PUBLIC external/googletest/googletest/include)
target_link_libraries(Tests PUBLIC gtest gtest_main)

target_include_directories(
    Tests PUBLIC ../../Nexus/src
)


target_link_libraries(Tests PUBLIC Nexus)

add_test(
    NAME Tests
    COMMAND Tests
)