##############################################################################
# Target
##############################################################################

set(header_files
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
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/include"
  PREFIX "Header Files"
  FILES ${header_files}
)

set(source_files
  src/alloy/render/mesh/mesh.cpp
  src/alloy/render/mesh/2d/polygon_2d_builder.cpp
  src/alloy/render/mesh/3d/polygon_3d_builder.cpp
  src/alloy/render/mesh/3d/cube_builder.cpp
  src/alloy/render/mesh/3d/sphere_builder.cpp
  src/alloy/render/mesh/unsafe_mesh_builder.hpp
  src/alloy/render/mesh/unsafe_mesh_builder.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}Render
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Render ALIAS ${PROJECT_NAME}Render)

target_compile_features(${PROJECT_NAME}Render
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}Render
  PROPERTIES
    OUTPUT_NAME "alloy-render"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Component Targets"
)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME}Render
    PRIVATE ALLOY_RENDER_EXPORT=1
  )
else()
  target_compile_definitions(${PROJECT_NAME}Render
    PUBLIC ALLOY_RENDER_STATIC=1
  )
endif()

target_include_directories(${PROJECT_NAME}Render
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}Render
  PUBLIC ${PROJECT_NAME}::Core
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}Render.SelfContainmentTest
    TARGET ${PROJECT_NAME}Render
    HEADERS ${header_files}
  )
endif ()

# Unit Tests

#if (ALLOY_COMPILE_TESTS)
#  add_subdirectory("test")
#endif ()

##############################################################################
# Installation
##############################################################################

if (ALLOY_IS_SUBPROJECT)
  return()
endif ()

include(GNUInstallDirs)

set(ALLOY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Targets
install(
  TARGETS "${PROJECT_NAME}Render"
  EXPORT "${PROJECT_NAME}RenderTargets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}RenderTargets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${ALLOY_CMAKE_CONFIG_DESTINATION}"
)

# Includes
install(
  DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(
  DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
