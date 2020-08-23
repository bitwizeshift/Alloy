

set(source_files
  src/use_discrete.cpp
)

add_library(${PROJECT_NAME}DiscreteGPU STATIC
  ${source_files}
)

add_library(${PROJECT_NAME}::DiscreteGPU ALIAS ${PROJECT_NAME}DiscreteGPU)

set_target_properties(${PROJECT_NAME}DiscreteGPU
  PROPERTIES
    OUTPUT_NAME "alloy-extra-discrete-gpu"
    FOLDER "Optional Component Targets"
)
