##############################################################################
# Target
##############################################################################

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(header_files
  include/alloy/render/gl/shader/shader.hpp
  include/alloy/render/gl/shader/program.hpp
  include/alloy/render/gl/shader/program_utilities.hpp
  include/alloy/render/gl/shader/active_program.hpp
  include/alloy/render/gl/material/blend_equation.hpp
  include/alloy/render/gl/material/blend_factor.hpp
  include/alloy/render/gl/material/cull_face.hpp
  include/alloy/render/gl/material/depth_compare.hpp
  include/alloy/render/gl/mesh/primitive_topology.hpp
  include/alloy/render/gl/mesh/fill_mode.hpp
  include/alloy/render/gl/mesh/mesh.hpp
  include/alloy/render/gl/error.hpp
  include/alloy/render/gl/state_cache.hpp
)

set(source_files
  src/alloy/render/gl/shader/shader.cpp
  src/alloy/render/gl/shader/program.cpp
  src/alloy/render/gl/shader/program_utilities.cpp
  src/alloy/render/gl/mesh/mesh.cpp
  src/alloy/render/gl/error.cpp
  src/alloy/render/gl/state_cache.cpp
)

add_alloy_library(${PROJECT_NAME}RenderGL
  ALIAS       ${PROJECT_NAME}::Render::GL
  OUTPUT_NAME alloy-render-gl
  EXPORT_NAME Render::GL
  API_PREFIX  ALLOY_RENDER_GL
  HEADERS     ${header_files}
  SOURCES     ${source_files}
  DEPENDENCIES
    PUBLIC
      ${PROJECT_NAME}::Core
      ${PROJECT_NAME}::IO
      ${PROJECT_NAME}::Render
      OpenGL::GL
      GLEW::GLEW
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
  TARGETS "${PROJECT_NAME}RenderGL"
  EXPORT AlloyTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
