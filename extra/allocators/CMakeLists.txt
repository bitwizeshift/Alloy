##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/extra/allocators/pointer_util.hpp
  include/alloy/extra/allocators/memory_arena.hpp
  include/alloy/extra/allocators/freelist.hpp
)

set(source_files
  src/alloy/extra/allocators/dummy.cpp
)

add_library(${PROJECT_NAME}ExtraAllocators
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Extra::Allocators ALIAS
  ${PROJECT_NAME}ExtraAllocators
)

target_compile_features(${PROJECT_NAME}ExtraAllocators
  PUBLIC cxx_std_17
)
target_link_libraries(${PROJECT_NAME}ExtraAllocators
  PUBLIC ${PROJECT_NAME}::Core
)
set_target_properties(${PROJECT_NAME}ExtraAllocators
  PROPERTIES
    OUTPUT_NAME "alloy-extra-allocators"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME}ExtraAllocators
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}ExtraAllocators.SelfContainmentTest
    TARGET ${PROJECT_NAME}ExtraAllocators
    HEADERS ${header_files}
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  # add_subdirectory("example")
endif ()