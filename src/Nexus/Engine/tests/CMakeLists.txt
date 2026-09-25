include (GoogleTest)

add_executable(EngineUnitTests
    src/CircleTests.cpp
    src/GraphicsTests.cpp
    src/Point2DTests.cpp
    src/Point3DTests.cpp
    src/TextureImporterTests.cpp 
    src/FileResourceLoaderTests.cpp
    src/SPIRVCrossShaderCompilerTests.cpp 
    src/MemoryResourceLoaderTests.cpp 
    src/ShaderPreprocessorTests.cpp)

target_link_libraries(EngineUnitTests 
    PRIVATE 
    Nexus 
    GTest::gtest 
    GTest::gtest_main
)

if(EMSCRIPTEN)
    target_link_options(EngineUnitTests PRIVATE
        "-sALLOW_MEMORY_GROWTH=1"
    )
endif()

copy_runtime_deps(EngineUnitTests)
copy_directory_to_target_output(EngineUnitTests resources)

gtest_discover_tests(EngineUnitTests DISCOVERY_MODE PRE_TEST)