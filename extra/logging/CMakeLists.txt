##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/extra/logging/logger.hpp
)

set(source_files
  src/alloy/extra/logging/logger.cpp
)

add_library(${PROJECT_NAME}ExtraLogging
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Extra::Logging ALIAS
  ${PROJECT_NAME}ExtraLogging
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
)

target_include_directories(${PROJECT_NAME}ExtraLogging
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}ExtraLogging
  PUBLIC ${PROJECT_NAME}::Core
  PUBLIC ${PROJECT_NAME}::IO
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}ExtraLogging.SelfContainmentTest
    TARGET ${PROJECT_NAME}ExtraLogging
    HEADERS ${header_files}
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)

endif ()