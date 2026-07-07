add_executable(LoggingUnitTests
    src/LoggingTests.cpp)   

target_link_libraries(LoggingUnitTests 
    PRIVATE 
    Nexus-Logging 
    GTest::gtest 
    GTest::gtest_main
)

add_test(
    NAME LoggingUnitTests
    COMMAND LoggingUnitTests
)