include (GoogleTest)

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

if(EMSCRIPTEN)
    target_link_options(CoreUnitTests PRIVATE
        "-sALLOW_MEMORY_GROWTH=1"
    )
endif()

copy_runtime_deps(CoreUnitTests)
gtest_discover_tests(CoreUnitTests DISCOVERY_MODE PRE_TEST)