add_executable(EngineUnitTests
    src/CircleTests.cpp
    src/GraphicsTests.cpp
    src/Point2DTests.cpp
    src/Point3DTests.cpp
    src/TextureImporterTests.cpp 
    src/FileResourceLoaderTests.cpp
)

target_link_libraries(EngineUnitTests 
    PRIVATE 
    Nexus 
    GTest::gtest 
    GTest::gtest_main
)

copy_runtime_deps(EngineUnitTests)
copy_directory_to_target_output(EngineUnitTests resources)

add_test(
    NAME EngineUnitTests
    COMMAND EngineUnitTests
)