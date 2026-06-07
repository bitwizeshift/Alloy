##############################################################################
# Target
##############################################################################

set(source_files
  src/alloy/io/event.test.cpp
  src/alloy/io/sequence_detector.test.cpp
)

add_alloy_test(${PROJECT_NAME}IOTest
  ALIAS             ${PROJECT_NAME}::IO::Test
  OUTPUT_NAME       alloy-io-test
  SOURCES           ${source_files}
  LIBRARIES         ${PROJECT_NAME}::IO
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)
