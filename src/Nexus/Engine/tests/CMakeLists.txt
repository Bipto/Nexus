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
    set_target_properties(
        EngineUnitTests
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

copy_runtime_deps(EngineUnitTests)
copy_directory_to_target_output(EngineUnitTests resources)

gtest_discover_tests(EngineUnitTests DISCOVERY_MODE PRE_TEST)