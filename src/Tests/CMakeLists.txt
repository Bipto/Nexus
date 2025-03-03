cmake_minimum_required(VERSION 3.30)

project("Tests")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG c00fd25
)

FetchContent_MakeAvailable(googletest)

add_executable(Tests src/main.cpp)
target_link_libraries(Tests PRIVATE gtest gtest_main)
target_link_libraries(Tests PRIVATE Nexus)

add_test(
    NAME Tests
    COMMAND Tests
)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries_to_target_output(Tests)