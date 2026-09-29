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
    set_target_properties(
        LoggingUnitTests
        PROPERTIES
        LINK_FLAGS
            "--std=c++23
            -O3
            -sALLOW_MEMORY_GROWTH=1
            -sNODERAWSOCKETS=1
            -sUSE_WEBGL2=1
            -sFULL_ES2=1
            -sFULL_ES3=1
            -sNO_DISABLE_EXCEPTION_CATCHING
            -lopenal
            -lembind
            -pthread")
endif()

gtest_discover_tests(LoggingUnitTests DISCOVERY_MODE PRE_TEST)