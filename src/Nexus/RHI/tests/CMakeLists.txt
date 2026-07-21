add_executable(RHITests
    src/RHITests.cpp)   

target_link_libraries(RHITests 
    PRIVATE 
    GTest::gtest 
    GTest::gtest_main
)

add_test(
    NAME RHITests
    COMMAND RHITests
)