find_package(Catch2 REQUIRED)

##############################################################################
# Target
##############################################################################

set(source_files
  # Utilities
  src/alloy/core/utilities/scope_guard.test.cpp
  src/alloy/core/utilities/delegate.test.cpp
  src/alloy/core/utilities/casts.test.cpp

  # Containers
  src/alloy/core/containers/span.test.cpp

  # Traits
  src/alloy/core/traits/function_traits.test.cpp

  # Math
  src/alloy/core/math/vector/vector2.test.cpp
  src/alloy/core/math/vector/vector3.test.cpp

  src/main.cpp
)

add_executable(${PROJECT_NAME}CoreTest
  ${source_files}
)
add_executable(${PROJECT_NAME}::Core::Test ALIAS ${PROJECT_NAME}CoreTest)

target_link_libraries(${PROJECT_NAME}CoreTest
  PRIVATE Catch2::Catch2
  PRIVATE ${PROJECT_NAME}::Core
)
set_target_properties(${PROJECT_NAME}CoreTest
  PROPERTIES
    OUTPUT_NAME "alloy-core-test"
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
