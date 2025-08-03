cmake_minimum_required(VERSION 3.28)

project("Tests")

enable_testing()

add_subdirectory(external/googletest)
add_executable(Tests src/main.cpp)

add_test(
    NAME Tests
    COMMAND Tests
)
    
target_link_libraries(Tests 
    PRIVATE 
    Nexus 
    gtest 
    gtest_main)

nexus_copy_required_binaries()
nexus_copy_required_runtime_libraries()