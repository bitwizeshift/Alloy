##############################################################################
# Target
##############################################################################

set(source_files
  src/alloy/core/math/simd.benchmark.cpp
)

add_alloy_benchmark(${PROJECT_NAME}CoreBenchmark
  ALIAS       ${PROJECT_NAME}::Core::Benchmark
  OUTPUT_NAME alloy-core-benchmark
  SOURCES     ${source_files}
  LIBRARIES   ${PROJECT_NAME}::Core
)
