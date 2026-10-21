##############################################################################
# Target
##############################################################################

set(source_files
  src/alloy/platform/event.test.cpp
  src/alloy/platform/sequence_detector.test.cpp
  src/alloy/platform/filesystem/file.test.cpp
)

add_alloy_test(${PROJECT_NAME}IOTest
  ALIAS             ${PROJECT_NAME}::Platform::Test
  OUTPUT_NAME       alloy-platform-test
  SOURCES           ${source_files}
  LIBRARIES         ${PROJECT_NAME}::Platform
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)
