find_package(Catch2)

##############################################################################
# Target
##############################################################################

set(source_files
  src/main.cpp
)

add_executable(${PROJECT_NAME}IOTest
  ${source_files}
)
add_executable(${PROJECT_NAME}::IO::Test ALIAS ${PROJECT_NAME}IOTest)

target_link_libraries(${PROJECT_NAME}IOTest
  PRIVATE Catch2::Catch2
  PRIVATE ${PROJECT_NAME}::IO
)
set_target_properties(${PROJECT_NAME}IOTest
  PROPERTIES
    OUTPUT_NAME "alloy-io-test"
)

##############################################################################
# CTest
##############################################################################

include(Catch)
catch_discover_tests(${PROJECT_NAME}IOTest
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)