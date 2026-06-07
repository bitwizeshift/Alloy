set(source_files
  src/use_discrete.cpp
)

add_alloy_library(${PROJECT_NAME}DiscreteGPU
  ALIAS       ${PROJECT_NAME}::DiscreteGPU
  OUTPUT_NAME alloy-extra-discrete-gpu
  FOLDER      "Optional Component Targets"
  STATIC
  SOURCES     ${source_files}
)
