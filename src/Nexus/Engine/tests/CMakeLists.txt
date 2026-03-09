FetchContent_MakeAvailable(googletest)

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

add_test(
    NAME EngineUnitTests
    COMMAND EngineUnitTests
)