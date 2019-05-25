##############################################################################
# Configured Files
##############################################################################

# read license, indent each line by 2 spaces
file(READ "${PROJECT_SOURCE_DIR}/LICENSE" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

# Generate Files
configure_file(
  "${ALLOY_MODULE_PATH}/version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/version.hpp"
  @ONLY
)

set(ALLOY_PRECISION "ALLOY_PRECISION_${ALLOY_PRECISION}")
string(TOUPPER "${ALLOY_PRECISION}" ALLOY_PRECISION)

configure_file(
  "${ALLOY_MODULE_PATH}/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/config.hpp"
  @ONLY
)

##############################################################################
# Target
##############################################################################

set(generated_header_files
  ${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/version.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/config.hpp
)

set(header_files
  # Core
  include/alloy/core/assert.hpp
  include/alloy/core/intrinsics.hpp
  include/alloy/core/macros.hpp
  include/alloy/core/precision.hpp
  include/alloy/core/precision/fixed.hpp
  include/alloy/core/precision/half.hpp
  include/alloy/core/precision/real.hpp

  # Utilities
  include/alloy/core/utilities/not_null.hpp
  include/alloy/core/utilities/piecewise_compare.hpp
  include/alloy/core/utilities/scope_guard.hpp

  # Math
  include/alloy/core/math/angle.hpp
  include/alloy/core/math/angle/basic_angle.hpp
  include/alloy/core/math/angle/degree.hpp
  include/alloy/core/math/angle/gradian.hpp
  include/alloy/core/math/angle/radian.hpp
  include/alloy/core/math/basis.hpp
  include/alloy/core/math/basis/basis2.hpp
  include/alloy/core/math/basis/basis3.hpp
  include/alloy/core/math/euler_angles.hpp
  include/alloy/core/math/interpolation.hpp
  include/alloy/core/math/math_constants.hpp
  include/alloy/core/math/math.hpp
  include/alloy/core/math/matrix.hpp
  include/alloy/core/math/matrix/matrix_utilities.hpp
  include/alloy/core/math/matrix/matrix2.hpp
  include/alloy/core/math/matrix/matrix3.hpp
  include/alloy/core/math/matrix/matrix4.hpp
  include/alloy/core/math/quaternion.hpp
  include/alloy/core/math/simplex.hpp
  include/alloy/core/math/trigonometry.hpp
  include/alloy/core/math/vector.hpp
  include/alloy/core/math/vector/vector_utilities.hpp
  include/alloy/core/math/vector/vector2.hpp
  include/alloy/core/math/vector/vector3.hpp
  include/alloy/core/math/vector/vector4.hpp

  # Geometry
  include/alloy/core/geometry/axis_aligned_box.hpp
  include/alloy/core/geometry/line.hpp
  include/alloy/core/geometry/line_segment.hpp
  include/alloy/core/geometry/plane.hpp
  include/alloy/core/geometry/point.hpp
  include/alloy/core/geometry/sphere.hpp
  include/alloy/core/geometry/ray.hpp

  # Model
  include/alloy/core/model/color.hpp
)

set(inline_header_files
  # Core
  include/alloy/core/intrinsics/clang.hpp
  include/alloy/core/intrinsics/gcc.hpp
  include/alloy/core/intrinsics/msvc.hpp
  include/alloy/core/precision/detail/real.inl

  # Math
  include/alloy/core/math/matrix/detail/matrix_utilities.inl
  include/alloy/core/math/matrix/detail/matrix2.inl
  include/alloy/core/math/matrix/detail/matrix3.inl
  include/alloy/core/math/matrix/detail/matrix4.inl
)

set(source_files
  # Math
  src/alloy/core/math/simplex.cpp
  src/alloy/core/math/trigonometry.cpp
  src/alloy/core/math/quaternion.cpp
  src/alloy/core/math/vector/vector2.cpp
  src/alloy/core/math/vector/vector3.cpp
  src/alloy/core/math/vector/vector4.cpp

  # Geometry
  src/alloy/core/geometry/axis_aligned_box.cpp
  src/alloy/core/geometry/line.cpp
  src/alloy/core/geometry/line_segment.cpp
  src/alloy/core/geometry/plane.cpp
  src/alloy/core/geometry/ray.cpp
  src/alloy/core/geometry/sphere.cpp

  # Model
  src/alloy/core/model/color.cpp
)

add_library(${PROJECT_NAME}Core
  ${generated_header_files}
  ${header_files}
  ${inline_header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Core ALIAS ${PROJECT_NAME}Core)

target_compile_features(${PROJECT_NAME}Core
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}Core
  PROPERTIES
    OUTPUT_NAME "alloy-core"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME}Core
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}Core.SelfContainmentTest
    TARGET ${PROJECT_NAME}Core
    HEADERS ${header_files}
  )
  target_sources(${PROJECT_NAME}Core.SelfContainmentTest PRIVATE ${inline_header_files})
endif ()

# Unit Tests

if (ALLOY_COMPILE_TESTS)
  add_subdirectory("test")
endif ()

##############################################################################
# Installation
##############################################################################

if (NOT NOT_SUBPROJECT)
  return()
endif ()

include(GNUInstallDirs)

set(ALLOY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Targets
install(
  TARGETS "${PROJECT_NAME}Core"
  EXPORT "${PROJECT_NAME}CoreTargets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}CoreTargets"
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
