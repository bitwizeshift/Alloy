##############################################################################
# Configured Files
##############################################################################

# read license, indent each line by 2 spaces
file(READ "${PROJECT_SOURCE_DIR}/LICENSE" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

set(ALLOY_CORE_PRECISION "ALLOY_CORE_PRECISION_${ALLOY_PRECISION}")
string(TOUPPER "${ALLOY_CORE_PRECISION}" ALLOY_CORE_PRECISION)

set(ALLOY_CORE_EXCEPTIONS_ENABLED ${ALLOY_ENABLE_EXCEPTIONS})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/config.hpp"
  @ONLY
)
unset(ALLOY_CORE_EXCEPTIONS_ENABLED)

##############################################################################
# Target
##############################################################################

set(generated_header_files
  ${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/config.hpp
)
source_group(
  TREE "${CMAKE_CURRENT_BINARY_DIR}/include"
  PREFIX "Generated Header Files"
  FILES ${generated_header_files}
)

set(header_files
  # Core
  include/alloy/core/api.hpp
  include/alloy/core/assert.hpp
  include/alloy/core/clamped.hpp
  include/alloy/core/saturated.hpp
  include/alloy/core/intrinsics.hpp
  include/alloy/core/macros.hpp
  include/alloy/core/type_index.hpp
  include/alloy/core/precision.hpp
  include/alloy/core/precision/fixed.hpp
  include/alloy/core/precision/half.hpp
  include/alloy/core/precision/real.hpp

  # Containers
  include/alloy/core/containers/deque.hpp
  include/alloy/core/containers/forward_list.hpp
  include/alloy/core/containers/hash_map.hpp
  include/alloy/core/containers/hash_multimap.hpp
  include/alloy/core/containers/hash_multiset.hpp
  include/alloy/core/containers/hash_set.hpp
  include/alloy/core/containers/list.hpp
  include/alloy/core/containers/map.hpp
  include/alloy/core/containers/multimap.hpp
  include/alloy/core/containers/multiset.hpp
  include/alloy/core/containers/priority_queue.hpp
  include/alloy/core/containers/queue.hpp
  include/alloy/core/containers/set.hpp
  include/alloy/core/containers/span.hpp
  include/alloy/core/containers/stack.hpp
  include/alloy/core/containers/string.hpp
  include/alloy/core/containers/vector.hpp

  # Traits
  include/alloy/core/traits/select_nth.hpp
  include/alloy/core/traits/function_traits.hpp
  include/alloy/core/traits/is_function_pointer.hpp

  # Utilities
  include/alloy/core/utilities/aligned_storage.hpp
  include/alloy/core/utilities/casts.hpp
  include/alloy/core/utilities/delegate.hpp
  include/alloy/core/utilities/signal.hpp
  include/alloy/core/utilities/expected.hpp
  include/alloy/core/utilities/explicitly_copyable.hpp
  include/alloy/core/utilities/fnv1a_hash.hpp
  include/alloy/core/utilities/hashed_string_view.hpp
  include/alloy/core/utilities/not_null.hpp
  include/alloy/core/utilities/piecewise_compare.hpp
  include/alloy/core/utilities/source_location.hpp
  include/alloy/core/utilities/scope_guard.hpp
  include/alloy/core/utilities/semantic_type.hpp
  include/alloy/core/utilities/string_view.hpp
  include/alloy/core/utilities/string_tokenizer.hpp
  include/alloy/core/utilities/string_utilities.hpp
  include/alloy/core/utilities/quantity.hpp

  # Memory
  include/alloy/core/memory/memory_resource.hpp
  include/alloy/core/memory/data_quantity.hpp
  include/alloy/core/memory/allocator.hpp
  include/alloy/core/memory/observer_ptr.hpp
  include/alloy/core/memory/unaligned.hpp
  include/alloy/core/memory/packed_buffer.hpp

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
  include/alloy/core/math/matrix/matrix2_constants.hpp
  include/alloy/core/math/matrix/matrix3_constants.hpp
  include/alloy/core/math/matrix/matrix4_constants.hpp
  include/alloy/core/math/quaternion.hpp
  include/alloy/core/math/quaternion_constants.hpp
  include/alloy/core/math/simplex.hpp
  include/alloy/core/math/trigonometry.hpp
  include/alloy/core/math/vector.hpp
  include/alloy/core/math/vector/vector_utilities.hpp
  include/alloy/core/math/vector/vector_streams.hpp
  include/alloy/core/math/vector/vector2.hpp
  include/alloy/core/math/vector/vector3.hpp
  include/alloy/core/math/vector/vector4.hpp
  include/alloy/core/math/vector/vector2_constants.hpp
  include/alloy/core/math/vector/vector3_constants.hpp
  include/alloy/core/math/vector/vector4_constants.hpp

  # Geometry
  include/alloy/core/geometry/axis_aligned_box.hpp
  include/alloy/core/geometry/intersection.hpp
  include/alloy/core/geometry/line.hpp
  include/alloy/core/geometry/line_segment.hpp
  include/alloy/core/geometry/plane.hpp
  include/alloy/core/geometry/point.hpp
  include/alloy/core/geometry/sphere.hpp
  include/alloy/core/geometry/ray.hpp

  # Model
  include/alloy/core/model/color.hpp
  include/alloy/core/model/color_constants.hpp
  include/alloy/core/model/image.hpp
  include/alloy/core/model/attribute_buffer_view.hpp
  include/alloy/core/model/light/light.hpp
  include/alloy/core/model/light/direction_light.hpp
  include/alloy/core/model/light/global_light.hpp
  include/alloy/core/model/light/point_light.hpp
  include/alloy/core/model/light/spot_light.hpp
  include/alloy/core/model/vertex.hpp
  include/alloy/core/model/normal.hpp
  include/alloy/core/model/texture_coordinate.hpp
  include/alloy/core/model/transform.hpp
  include/alloy/core/model/projection.hpp
  include/alloy/core/model/camera.hpp
  include/alloy/core/model/lazy_transform.hpp
  include/alloy/core/model/mesh.hpp
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/include"
  PREFIX "Header Files"
  FILES ${header_files}
)

set(source_files
  src/alloy/core/assert.cpp
  src/alloy/core/type_index.cpp

  # Utilities
  src/alloy/core/utilities/string_tokenizer.cpp
  src/alloy/core/utilities/string_utilities.cpp

  # Math
  src/alloy/core/math/simplex.cpp
  src/alloy/core/math/trigonometry.cpp
  src/alloy/core/math/quaternion.cpp
  src/alloy/core/math/vector/vector2.cpp
  src/alloy/core/math/vector/vector3.cpp
  src/alloy/core/math/vector/vector4.cpp
  src/alloy/core/math/matrix/matrix2.cpp
  src/alloy/core/math/matrix/matrix3.cpp
  src/alloy/core/math/matrix/matrix4.cpp

  # Geometry
  src/alloy/core/geometry/axis_aligned_box.cpp
  src/alloy/core/geometry/line.cpp
  src/alloy/core/geometry/line_segment.cpp
  src/alloy/core/geometry/plane.cpp
  src/alloy/core/geometry/ray.cpp
  src/alloy/core/geometry/sphere.cpp

  # Memory
  src/alloy/core/memory/packed_buffer.cpp

  # Model
  src/alloy/core/model/attribute_buffer_view.cpp
  src/alloy/core/model/color.cpp
  src/alloy/core/model/image.cpp
  src/alloy/core/model/transform.cpp
  src/alloy/core/model/projection.cpp
  src/alloy/core/model/camera.cpp
  src/alloy/core/model/mesh.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}Core
  ${generated_header_files}
  ${header_files}
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
    FOLDER "Component Targets"
)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME}Core
    PRIVATE ALLOY_CORE_EXPORT=1
  )
else()
  target_compile_definitions(${PROJECT_NAME}Core
    PUBLIC ALLOY_CORE_STATIC=1
  )
endif()

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
endif ()

# Unit Tests

if (ALLOY_COMPILE_TESTS)
  add_subdirectory("test")
endif ()

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
