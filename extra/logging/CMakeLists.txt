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
add_alloy_library(${PROJECT_NAME}ExtraLogging
  ALIAS       ${PROJECT_NAME}::Extra::Logging
  OUTPUT_NAME alloy-extra-logging
  FOLDER      "Optional Component Targets"
  HEADERS     ${header_files}
  SOURCES     ${source_files}
  DEPENDENCIES
    PUBLIC
      fmt::fmt
      ${PROJECT_NAME}::Core
      ${PROJECT_NAME}::IO
)

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example")
endif ()
