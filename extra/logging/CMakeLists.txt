##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/extra/logging/log_filter.hpp
  include/alloy/extra/logging/log_level.hpp
  include/alloy/extra/logging/logger.hpp
  include/alloy/extra/logging/console_log_stream.hpp
  include/alloy/extra/logging/file_log_stream.hpp
)

set(source_files
  src/alloy/extra/logging/logger.cpp
  src/alloy/extra/logging/console_log_stream.cpp
  src/alloy/extra/logging/file_log_stream.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}ExtraLogging)
add_library(${PROJECT_NAME}::Extra::Logging ALIAS
  ${PROJECT_NAME}ExtraLogging
)

target_sources(${PROJECT_NAME}ExtraLogging
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include
      FILES ${header_files}
  PRIVATE
    ${source_files}
)

target_compile_features(${PROJECT_NAME}ExtraLogging
  PUBLIC cxx_std_17
)
target_link_libraries(${PROJECT_NAME}ExtraLogging
  PUBLIC fmt::fmt
)
set_target_properties(${PROJECT_NAME}ExtraLogging
  PROPERTIES
    OUTPUT_NAME "alloy-extra-logging"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Optional Component Targets"
)

target_link_libraries(${PROJECT_NAME}ExtraLogging
  PUBLIC ${PROJECT_NAME}::Core
  PUBLIC ${PROJECT_NAME}::IO
)

# Header Self-Containment Tests

include(SurrogateTarget)

if (ALLOY_COMPILE_SURROGATE_TESTS)
  add_surrogate_target(${PROJECT_NAME}ExtraLogging.Surrogate
    TARGET ${PROJECT_NAME}ExtraLogging
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example")
endif ()
