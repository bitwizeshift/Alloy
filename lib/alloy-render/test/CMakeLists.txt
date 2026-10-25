##############################################################################
# Target
##############################################################################

set(source_files
  src/alloy/render/device.test.cpp

  # Mesh
  src/alloy/render/mesh/mesh_test_utilities.hpp
  src/alloy/render/mesh/mesh.test.cpp
  src/alloy/render/mesh/unsafe_mesh_builder.test.cpp
  src/alloy/render/mesh/2d/polygon_2d_builder.test.cpp
  src/alloy/render/mesh/3d/cube_builder.test.cpp
  src/alloy/render/mesh/3d/polygon_3d_builder.test.cpp
  src/alloy/render/mesh/3d/sphere_builder.test.cpp

  # Pass
  src/alloy/render/pass/render_batch.test.cpp

  # Resource
  src/alloy/render/resource/handle_pool.test.cpp

  # Shader
  src/alloy/render/shader/program_layout.test.cpp
  src/alloy/render/shader/resolved_parameters.test.cpp

  # Texture
  src/alloy/render/texture/texture_spec.test.cpp
)

add_alloy_test(${PROJECT_NAME}RenderTest
  ALIAS       ${PROJECT_NAME}::Render::Test
  OUTPUT_NAME alloy-render-test
  SOURCES     ${source_files}
  LIBRARIES   ${PROJECT_NAME}::Render
)
