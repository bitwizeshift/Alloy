find_package(Catch2)

##############################################################################
# Target
##############################################################################

set(source_files
  # Math
  src/alloy/core/math/vector/vector2.test.cpp

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

##############################################################################
# CTest
##############################################################################

include(Catch)
catch_discover_tests(${PROJECT_NAME}CoreTest
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)