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

    set_target_properties(
        CoreUnitTests
        PROPERTIES
        LINK_FLAGS
            "--std=c++23
            -O3
            -sALLOW_MEMORY_GROWTH=1
            -sUSE_WEBGL2=1
            -sFULL_ES2=1
            -sFULL_ES3=1
            -sNO_DISABLE_EXCEPTION_CATCHING
            -lopenal
            -lembind
            -pthread"
    )
endif()

copy_runtime_deps(CoreUnitTests)
gtest_discover_tests(CoreUnitTests DISCOVERY_MODE PRE_TEST)