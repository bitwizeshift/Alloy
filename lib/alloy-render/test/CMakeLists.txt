##############################################################################
# Target
##############################################################################

set(source_files
  # Mesh
  src/alloy/render/mesh/mesh.test.cpp
  src/alloy/render/mesh/unsafe_mesh_builder.test.cpp
  src/alloy/render/mesh/2d/polygon_2d_builder.test.cpp
  src/alloy/render/mesh/3d/polygon_3d_builder.test.cpp
)

add_alloy_test(${PROJECT_NAME}RenderTest
  ALIAS       ${PROJECT_NAME}::Render::Test
  OUTPUT_NAME alloy-render-test
  SOURCES     ${source_files}
  LIBRARIES   ${PROJECT_NAME}::Render
)
