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

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}RenderGL)
add_library(${PROJECT_NAME}::Render::GL ALIAS ${PROJECT_NAME}RenderGL)

target_sources(${PROJECT_NAME}RenderGL
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include
      FILES ${header_files}
  PRIVATE
    ${source_files}
)

target_compile_features(${PROJECT_NAME}RenderGL
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}RenderGL
  PROPERTIES
    OUTPUT_NAME "alloy-render-gl"
    EXPORT_NAME "Render::GL"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Component Targets"
)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME}RenderGL
    PRIVATE ALLOY_RENDER_GL_EXPORT=1
  )
else()
  target_compile_definitions(${PROJECT_NAME}RenderGL
    PUBLIC ALLOY_RENDER_GL_STATIC=1
  )
endif()

target_link_libraries(${PROJECT_NAME}RenderGL
  PUBLIC ${PROJECT_NAME}::Core
  PUBLIC ${PROJECT_NAME}::IO
  PUBLIC ${PROJECT_NAME}::Render
  PUBLIC OpenGL::GL
  PUBLIC GLEW::GLEW
)

# Header Self-Containment Tests

include(SurrogateTarget)

if (ALLOY_COMPILE_SURROGATE_TESTS)
  add_surrogate_target(${PROJECT_NAME}RenderGL.Surrogate
    TARGET ${PROJECT_NAME}RenderGL
  )
endif ()

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
