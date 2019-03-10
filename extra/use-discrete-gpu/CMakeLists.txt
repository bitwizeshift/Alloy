

set(source_files
  src/use_discrete.cpp
)

add_library(${PROJECT_NAME}DiscreteGPU STATIC
  ${source_files}
)

add_library(${PROJECT_NAME}::DiscreteGPU ALIAS ${PROJECT_NAME}DiscreteGPU)

target_link_libraries(${PROJECT_NAME}DiscreteGPU
  PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
)