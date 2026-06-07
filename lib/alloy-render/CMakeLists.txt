##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/render/light/light.hpp
  include/alloy/render/light/direction_light.hpp
  include/alloy/render/light/global_light.hpp
  include/alloy/render/light/point_light.hpp
  include/alloy/render/light/spot_light.hpp
  include/alloy/render/material/blend_factor.hpp
  include/alloy/render/material/blend_equation.hpp
  include/alloy/render/material/depth_compare.hpp
  include/alloy/render/material/cull_face.hpp
  include/alloy/render/mesh/fill_mode.hpp
  include/alloy/render/mesh/primitive_topology.hpp
  include/alloy/render/mesh/mesh.hpp
  include/alloy/render/mesh/mesh_builder.hpp
  include/alloy/render/mesh/2d/polygon_2d_builder.hpp
  include/alloy/render/mesh/3d/polygon_3d_builder.hpp
  include/alloy/render/mesh/3d/cube_builder.hpp
  include/alloy/render/mesh/3d/sphere_builder.hpp
  include/alloy/render/mesh/attribute_locations.hpp
  include/alloy/render/mesh/unsafe_mesh_builder.hpp
)

set(source_files
  src/alloy/render/mesh/mesh.cpp
  src/alloy/render/mesh/2d/polygon_2d_builder.cpp
  src/alloy/render/mesh/3d/polygon_3d_builder.cpp
  src/alloy/render/mesh/3d/cube_builder.cpp
  src/alloy/render/mesh/3d/sphere_builder.cpp
  src/alloy/render/mesh/unsafe_mesh_builder.cpp
  src/alloy/render/mesh/attribute_packer.hpp
)

add_alloy_library(${PROJECT_NAME}Render
  ALIAS       ${PROJECT_NAME}::Render
  OUTPUT_NAME alloy-render
  EXPORT_NAME Render
  API_PREFIX  ALLOY_RENDER
  HEADERS     ${header_files}
  SOURCES     ${source_files}
  DEPENDENCIES
    PUBLIC ${PROJECT_NAME}::Core
)

# Unit Tests

#if (ALLOY_COMPILE_TESTS)
#  add_subdirectory("test")
#endif ()

##############################################################################
# Installation
##############################################################################

# Installation is currently disabled: exporting AlloyEngine pulls the vendored
# tga-lib into the AlloyTargets export set, which is not yet packaged. The
# install rules below are kept modern and ready for when that is resolved.
return ()

include(GNUInstallDirs)

install(
  TARGETS "${PROJECT_NAME}Render"
  EXPORT AlloyTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
