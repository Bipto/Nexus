add_executable(RHIBenchmarks
    src/RHIBenchmarks.cpp)   

target_link_libraries(RHIBenchmarks 
    PRIVATE 
    benchmark::benchmark
)

add_test(
    NAME RHIBenchmarks
    COMMAND RHIBenchmarks
)

find_package(Threads REQUIRED)
target_link_libraries(RHIBenchmarks PRIVATE Threads::Threads)