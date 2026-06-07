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

set(header_files
  # Core
  include/alloy/core/api.hpp
  include/alloy/core/assert.hpp
  include/alloy/core/source_location.hpp
  include/alloy/core/types.hpp

  include/alloy/core/math/clamped.hpp
  include/alloy/core/math/saturated.hpp
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
  include/alloy/core/containers/map_view.hpp
  include/alloy/core/containers/multimap.hpp
  include/alloy/core/containers/multiset.hpp
  include/alloy/core/containers/priority_queue.hpp
  include/alloy/core/containers/queue.hpp
  include/alloy/core/containers/set.hpp
  include/alloy/core/containers/set_view.hpp
  include/alloy/core/containers/span.hpp
  include/alloy/core/containers/stack.hpp
  include/alloy/core/containers/vector.hpp
  include/alloy/core/containers/polymorphic_vector.hpp

  # Traits
  include/alloy/core/traits/always_false.hpp
  include/alloy/core/traits/select_nth.hpp
  include/alloy/core/traits/function_traits.hpp
  include/alloy/core/traits/is_function_pointer.hpp
  include/alloy/core/traits/for_each_member.hpp
  include/alloy/core/traits/detected.hpp
  include/alloy/core/traits/common_reference.hpp
  include/alloy/core/traits/copy_cv.hpp
  include/alloy/core/traits/copy_cvref.hpp

  # Utilities
  include/alloy/core/utilities/aligned_storage.hpp
  include/alloy/core/utilities/int_utilities.hpp
  include/alloy/core/utilities/casts.hpp
  include/alloy/core/utilities/compare.hpp
  include/alloy/core/utilities/delegate.hpp
  include/alloy/core/utilities/signal.hpp
  include/alloy/core/utilities/result.hpp
  include/alloy/core/utilities/explicitly_copyable.hpp
  include/alloy/core/utilities/fnv1a_hash.hpp
  include/alloy/core/utilities/not_null.hpp
  include/alloy/core/utilities/piecewise_compare.hpp
  include/alloy/core/utilities/scope_guard.hpp
  include/alloy/core/utilities/semantic_type.hpp
  include/alloy/core/utilities/quantity.hpp
  include/alloy/core/utilities/optional.hpp
  include/alloy/core/utilities/option_set.hpp
  include/alloy/core/utilities/ctor_function.hpp
  include/alloy/core/utilities/enum_traits.hpp
  include/alloy/core/utilities/hash.hpp
  include/alloy/core/utilities/mixins.hpp

  # Algorithms
  include/alloy/core/algorithms/compare_reduce.hpp
  include/alloy/core/algorithms/max.hpp
  include/alloy/core/algorithms/min.hpp

  # Strings
  include/alloy/core/string/encoding/ansi_encoding.hpp
  include/alloy/core/string/encoding/ascii_encoding.hpp
  include/alloy/core/string/encoding/encoding_converter.hpp
  include/alloy/core/string/encoding/encoding_traits.hpp
  include/alloy/core/string/encoding/encoding_utilities.hpp
  include/alloy/core/string/encoding/latin1_encoding.hpp
  include/alloy/core/string/encoding/utf16_encoding.hpp
  include/alloy/core/string/encoding/utf32_encoding.hpp
  include/alloy/core/string/encoding/utf8_encoding.hpp
  include/alloy/core/string/encoding/wide_encoding.hpp
  include/alloy/core/string/hashed_string_view.hpp
  include/alloy/core/string/string_view.hpp
  include/alloy/core/string/string_tokenizer.hpp
  include/alloy/core/string/string_utilities.hpp
  include/alloy/core/string/string.hpp
  include/alloy/core/string/zstring_view.hpp

  # Memory
  include/alloy/core/memory/memory_resource.hpp
  include/alloy/core/memory/data_quantity.hpp
  include/alloy/core/memory/allocator.hpp
  include/alloy/core/memory/observer_ptr.hpp
  include/alloy/core/memory/unaligned.hpp
  include/alloy/core/memory/packed_buffer.hpp
  include/alloy/core/memory/object_storage.hpp

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
  include/alloy/core/math/simd.hpp
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

  # Threads
  include/alloy/core/thread/thread_group.hpp

  # Geometry
  include/alloy/core/geometry/point/barycentric.hpp
  include/alloy/core/geometry/point/point2.hpp
  include/alloy/core/geometry/point/point3.hpp
  include/alloy/core/geometry/point/polar.hpp
  include/alloy/core/geometry/point/spherical.hpp
  include/alloy/core/geometry/axis_aligned_box.hpp
  include/alloy/core/geometry/frustum.hpp
  include/alloy/core/geometry/intersection.hpp
  include/alloy/core/geometry/line.hpp
  include/alloy/core/geometry/line_segment.hpp
  include/alloy/core/geometry/plane.hpp
  include/alloy/core/geometry/sphere.hpp
  include/alloy/core/geometry/ray.hpp

  # Model
  include/alloy/core/model/color.hpp
  include/alloy/core/model/color_constants.hpp
  include/alloy/core/model/image.hpp
  include/alloy/core/model/transform.hpp
  include/alloy/core/model/projection.hpp
  include/alloy/core/model/camera.hpp
  include/alloy/core/model/lazy_transform.hpp
  include/alloy/core/model/interpolated_transform.hpp
)

set(source_files
  src/alloy/core/assert.cpp
  src/alloy/core/type_index.cpp

  # Strings
  src/alloy/core/string/string_tokenizer.cpp
  src/alloy/core/string/string_utilities.cpp

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
  src/alloy/core/geometry/frustum.cpp
  src/alloy/core/geometry/line.cpp
  src/alloy/core/geometry/line_segment.cpp
  src/alloy/core/geometry/plane.cpp
  src/alloy/core/geometry/ray.cpp
  src/alloy/core/geometry/sphere.cpp

  # Memory
  src/alloy/core/memory/packed_buffer.cpp

  # Model
  src/alloy/core/model/color.cpp
  src/alloy/core/model/image.cpp
  src/alloy/core/model/transform.cpp
  src/alloy/core/model/projection.cpp
  src/alloy/core/model/camera.cpp

)

add_alloy_library(${PROJECT_NAME}Core
  ALIAS             ${PROJECT_NAME}::Core
  OUTPUT_NAME       alloy-core
  EXPORT_NAME       Core
  API_PREFIX        ALLOY_CORE
  HIDDEN_VISIBILITY
  HEADERS           ${header_files}
  GENERATED_HEADERS ${generated_header_files}
  SOURCES           ${source_files}
)

# Unit Tests

if (ALLOY_COMPILE_TESTS)
  add_subdirectory("test")
endif ()
if (ALLOY_COMPILE_BENCHMARKS)
  add_subdirectory("benchmark")
endif ()

##############################################################################
# Installation
##############################################################################

# Installation is currently disabled: exporting AlloyEngine pulls the vendored
# tga-lib into the AlloyTargets export set, which is not yet packaged. The
# install rules below are kept modern and ready for when that is resolved.
return ()

include(GNUInstallDirs)

install(
  TARGETS "${PROJECT_NAME}Core"
  EXPORT AlloyTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILE_SET generated DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
