cmake_minimum_required(VERSION 3.28)

project("TestRunner")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 6986c2b
)
FetchContent_MakeAvailable(googletest)

add_executable(TestRunner src/main.cpp)

add_test(
    NAME AllTests
    COMMAND $<TARGET_FILE:TestRunner>
)
    
target_link_libraries(TestRunner 
    PRIVATE 
    Nexus 
    gtest 
    gtest_main)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()