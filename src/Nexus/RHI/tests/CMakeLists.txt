add_executable(RHITests
    include/CommandListStorageTestHelpers.hpp
    src/CommandListStorageBasicTests.cpp
    src/CommandListStorageDrawTests.cpp
    src/CommandListStorageCopyTests.cpp)   

target_link_libraries(RHITests 
    PRIVATE 
    GTest::gtest 
    GTest::gtest_main
    Nexus-RHI
)

target_include_directories(RHITests
    PRIVATE
    include
)

add_test(
    NAME RHITests
    COMMAND RHITests
)