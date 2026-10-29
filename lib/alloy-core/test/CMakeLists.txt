##############################################################################
# Target
##############################################################################

set(source_files
  # Utilities
  src/alloy/core/utilities/scope_guard.test.cpp
  src/alloy/core/utilities/delegate.test.cpp
  src/alloy/core/utilities/casts.test.cpp
  src/alloy/core/utilities/result.test.cpp
  src/alloy/core/utilities/result.constexpr.test.cpp
  src/alloy/core/utilities/failure.test.cpp
  src/alloy/core/utilities/hash.test.cpp
  src/alloy/core/utilities/int_utilities.test.cpp

  # Strings
  src/alloy/core/string/encoding/ansi_encoding.test.cpp
  src/alloy/core/string/encoding/encoding_utilities.test.cpp
  src/alloy/core/string/encoding/latin1_encoding.test.cpp
  src/alloy/core/string/encoding/utf16_encoding.test.cpp
  src/alloy/core/string/encoding/utf32_encoding.test.cpp
  src/alloy/core/string/encoding/utf8_encoding.test.cpp
  src/alloy/core/string/encoding/wide_encoding.test.cpp
  src/alloy/core/string/string_tokenizer.test.cpp

  # Containers
  src/alloy/core/containers/map_view.test.cpp
  src/alloy/core/containers/set_view.test.cpp
  src/alloy/core/containers/polymorphic_vector.test.cpp

  # Traits
  src/alloy/core/traits/function_traits.test.cpp

  # Geometry
  src/alloy/core/geometry/sphere.test.cpp
  src/alloy/core/geometry/triangle.test.cpp
  src/alloy/core/geometry/axis_aligned_box.test.cpp
  src/alloy/core/geometry/capsule.test.cpp
  src/alloy/core/geometry/circle.test.cpp
  src/alloy/core/geometry/frustum.test.cpp
  src/alloy/core/geometry/intersection.test.cpp
  src/alloy/core/geometry/line.test.cpp
  src/alloy/core/geometry/line_segment.test.cpp
  src/alloy/core/geometry/oriented_box.test.cpp
  src/alloy/core/geometry/plane.test.cpp
  src/alloy/core/geometry/point/barycentric.test.cpp
  src/alloy/core/geometry/point/point2.test.cpp
  src/alloy/core/geometry/point/polar.test.cpp
  src/alloy/core/geometry/point/spherical.test.cpp
  src/alloy/core/geometry/ray.test.cpp
  src/alloy/core/geometry/raycast.test.cpp
  src/alloy/core/geometry/rectangle.test.cpp

  # Model
  src/alloy/core/model/cached_transform.test.cpp
  src/alloy/core/model/camera.test.cpp
  src/alloy/core/model/color.test.cpp
  src/alloy/core/model/image.test.cpp
  src/alloy/core/model/projection.test.cpp
  src/alloy/core/model/transform.test.cpp

  # Math
  src/alloy/core/math/vector/vector2.test.cpp
  src/alloy/core/math/vector/vector3.test.cpp
  src/alloy/core/math/vector/vector4.test.cpp
  src/alloy/core/math/vector/vector_utilities.test.cpp
  src/alloy/core/math/angle/basic_angle.test.cpp
  src/alloy/core/math/clamped.test.cpp
  src/alloy/core/math/dual_quaternion.test.cpp
  src/alloy/core/math/euler_angles.test.cpp
  src/alloy/core/math/interpolation.test.cpp
  src/alloy/core/math/math.test.cpp
  src/alloy/core/math/quaternion.test.cpp
  src/alloy/core/math/saturated.test.cpp
  src/alloy/core/math/simd.test.cpp
  src/alloy/core/math/simplex.test.cpp
  src/alloy/core/math/trigonometry.test.cpp
  src/alloy/core/math/matrix/matrix2.test.cpp
  src/alloy/core/math/matrix/matrix3.test.cpp
  src/alloy/core/math/matrix/matrix4.test.cpp
  src/alloy/core/math/matrix/matrix_utilities.test.cpp

  # Memory
  src/alloy/core/memory/packed_buffer.test.cpp

  # IO
  src/alloy/core/io/back_insert_writer.test.cpp
  src/alloy/core/io/buffered_reader.test.cpp
  src/alloy/core/io/buffered_writer.test.cpp
  src/alloy/core/io/deserializer.test.cpp
  src/alloy/core/io/error.test.cpp
  src/alloy/core/io/indirect_reader.test.cpp
  src/alloy/core/io/indirect_writer.test.cpp
  src/alloy/core/io/reader.test.cpp
  src/alloy/core/io/serializer.test.cpp
  src/alloy/core/io/span_reader.test.cpp
  src/alloy/core/io/test_doubles.hpp
  src/alloy/core/io/writer.test.cpp
)

add_alloy_test(${PROJECT_NAME}CoreTest
  ALIAS       ${PROJECT_NAME}::Core::Test
  OUTPUT_NAME alloy-core-test
  SOURCES     ${source_files}
  LIBRARIES   ${PROJECT_NAME}::Core
)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")

  # clang-cl complains about truncating 64 to 32 bits of values in the unit tests,
  # and about float equality comparisons.
  target_compile_options(${PROJECT_NAME}CoreTest
    PRIVATE
      -Wno-shorten-64-to-32
      -Wno-sign-conversion
      -Wno-unneeded-member-function
      -Wno-unused-member-function
      -Wno-float-equal
  )
endif ()
