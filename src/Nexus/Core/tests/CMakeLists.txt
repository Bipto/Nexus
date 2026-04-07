add_executable(CoreUnitTests
    src/NamedJThreadTests.cpp
    src/ResourcePoolTests.cpp
    src/ThreadPoolTests.cpp
    src/UtilsTests.cpp 
    src/AutoReleaseTests.cpp 
    src/MemorySizeConverterTests.cpp)   

target_link_libraries(CoreUnitTests 
    PRIVATE 
    Nexus-Core 
    GTest::gtest 
    GTest::gtest_main
)

copy_runtime_deps(CoreUnitTests)

add_test(
    NAME CoreUnitTests
    COMMAND CoreUnitTests
)