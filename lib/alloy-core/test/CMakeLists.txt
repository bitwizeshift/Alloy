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
  src/alloy/core/containers/span.test.cpp
  src/alloy/core/containers/set_view.test.cpp
  src/alloy/core/containers/polymorphic_vector.test.cpp

  # Traits
  src/alloy/core/traits/function_traits.test.cpp

  # Geometry
  src/alloy/core/geometry/sphere.test.cpp
  src/alloy/core/geometry/axis_aligned_box.test.cpp
  src/alloy/core/geometry/intersection.test.cpp

  # Model
  src/alloy/core/model/color.test.cpp

  # Math
  src/alloy/core/math/vector/vector2.test.cpp
  src/alloy/core/math/vector/vector3.test.cpp
  src/alloy/core/math/angle/basic_angle.test.cpp
  src/alloy/core/math/clamped.test.cpp
  src/alloy/core/math/saturated.test.cpp
  src/alloy/core/math/simd.test.cpp

  # Memory
  src/alloy/core/memory/packed_buffer.test.cpp

  src/main.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_executable(${PROJECT_NAME}CoreTest
  ${source_files}
)
add_executable(${PROJECT_NAME}::Core::Test ALIAS ${PROJECT_NAME}CoreTest)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")

  # clang-cl complains about truncating 64 to 32 bits of values in the unit tests.
  target_compile_options(${PROJECT_NAME}CoreTest
    PRIVATE
      -Wno-shorten-64-to-32
      -Wno-sign-conversion
      -Wno-unneeded-member-function
      -Wno-unused-member-function
  )
endif ()


target_link_libraries(${PROJECT_NAME}CoreTest
  PRIVATE Catch2::Catch2
  PRIVATE ${PROJECT_NAME}::Core
)
set_target_properties(${PROJECT_NAME}CoreTest
  PROPERTIES
    OUTPUT_NAME "alloy-core-test"
    FOLDER "Unit Test Targets"
)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME}CoreTest
    PRIVATE -Wno-float-equal
  )
endif ()

##############################################################################
# CTest
##############################################################################

include(Catch)
catch_discover_tests(${PROJECT_NAME}CoreTest)
