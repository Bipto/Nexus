cmake_minimum_required(VERSION 3.28)

project("TestRunner")

find_package(gtest CONFIG REQUIRED)

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

nexus_copy_required_binaries(TestRunner)
nexus_copy_required_runtime_libraries(TestRunner)