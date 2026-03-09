FetchContent_MakeAvailable(googletest)
include(GoogleTest)

mark_as_advanced(BUILD_GMOCK BUILD_GTEST gtest_hide_internal_symbols)

add_executable(EngineUnitTests
    src/CircleTests.cpp
    src/GraphicsTests.cpp
    src/Point2DTests.cpp
    src/Point3DTests.cpp
    src/ResourcePoolTests.cpp
)
    
target_link_libraries(EngineUnitTests 
    PRIVATE 
    Nexus 
    gtest 
    gtest_main
)

copy_runtime_deps(EngineUnitTests)

gtest_discover_tests(EngineUnitTests DISCOVERY_MODE PRE_TEST)