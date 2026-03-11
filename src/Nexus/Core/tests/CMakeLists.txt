FetchContent_MakeAvailable(googletest)

add_executable(CoreUnitTest
    "src/NamedJThreadTests.cpp"
)
    
target_link_libraries(CoreUnitTest 
    PRIVATE 
    Nexus-Core 
    gtest 
    gtest_main
)

copy_runtime_deps(CoreUnitTest)

add_test(
    NAME CoreUnitTest
    COMMAND CoreUnitTest
)