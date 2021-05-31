##############################################################################
# Target
##############################################################################

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(header_files
  include/alloy/render/gl/material/blend_equation.hpp
  include/alloy/render/gl/material/blend_factor.hpp
  include/alloy/render/gl/material/cull_face.hpp
  include/alloy/render/gl/material/depth_compare.hpp
  include/alloy/render/gl/mesh/primitive_topology.hpp
  include/alloy/render/gl/mesh/fill_mode.hpp
  include/alloy/render/gl/error.hpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/include"
  PREFIX "Header Files"
  FILES ${header_files}
)

set(source_files
  src/alloy/render/gl/error.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}RenderGL
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Render::GL ALIAS ${PROJECT_NAME}RenderGL)

target_compile_features(${PROJECT_NAME}RenderGL
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}RenderGL
  PROPERTIES
    OUTPUT_NAME "alloy-render-gl"
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

message("${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(${PROJECT_NAME}RenderGL
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}RenderGL
  PUBLIC ${PROJECT_NAME}::Core
  PUBLIC ${PROJECT_NAME}::Render
  PUBLIC OpenGL::GL
  PUBLIC GLEW::GLEW
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}RenderGL.SelfContainmentTest
    TARGET ${PROJECT_NAME}RenderGL
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
  TARGETS "${PROJECT_NAME}RenderGL"
  EXPORT "${PROJECT_NAME}RenderGLTargets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}RenderGLTargets"
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
