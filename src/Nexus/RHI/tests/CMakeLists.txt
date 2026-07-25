add_executable(RHITests
    include/CommandListStorageTestHelpers.hpp
    src/CommandListStorageAccelerationTests.cpp
    src/CommandListStorageBarrierTests.cpp
    src/CommandListStorageBasicTests.cpp
    src/CommandListStorageCopyTests.cpp
    src/CommandListStorageDebugTests.cpp
    src/CommandListStorageDrawTests.cpp
    src/CommandListStoragePushConstantsTests.cpp
    src/CommandListStorageResetTests.cpp
    src/CommandListStorageResourceTests.cpp
    src/CommandListStorageStateTests.cpp      
    src/CommandListStorageStressTests.cpp 
    src/CommandHeaderTests.cpp 
    src/CommandListStorageStreamTests.cpp)   

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