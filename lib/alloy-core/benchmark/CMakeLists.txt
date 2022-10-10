##############################################################################
# Target
##############################################################################

set(source_files
  src/main.cpp
  src/alloy/core/math/simd.benchmark.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_executable(${PROJECT_NAME}CoreBenchmark
  ${source_files}
)
add_executable(${PROJECT_NAME}::Core::Benchmark ALIAS ${PROJECT_NAME}CoreBenchmark)

target_compile_definitions(${PROJECT_NAME}CoreBenchmark
  PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING=1
)

target_link_libraries(${PROJECT_NAME}CoreBenchmark
  PRIVATE Catch2::Catch2
  PRIVATE ${PROJECT_NAME}::Core
)
set_target_properties(${PROJECT_NAME}CoreBenchmark
  PROPERTIES
    OUTPUT_NAME "alloy-core-benchmark"
    FOLDER "Benchmark Targets"
)

##############################################################################
# CTest
##############################################################################

include(Catch)
catch_discover_tests(${PROJECT_NAME}CoreBenchmark)
