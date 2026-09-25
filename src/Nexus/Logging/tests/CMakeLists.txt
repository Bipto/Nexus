include (GoogleTest)

add_executable(LoggingUnitTests
    src/LoggingTests.cpp)   

target_link_libraries(LoggingUnitTests 
    PRIVATE 
    Nexus-Logging 
    GTest::gtest 
    GTest::gtest_main
)

if(EMSCRIPTEN)
    target_link_options(LoggingUnitTests PRIVATE
        "-sALLOW_MEMORY_GROWTH=1"
    )
endif()

gtest_discover_tests(LoggingUnitTests DISCOVERY_MODE PRE_TEST)