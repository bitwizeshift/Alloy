##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/extra/allocators/pointer_util.hpp
  include/alloy/extra/allocators/memory_arena.hpp
  include/alloy/extra/allocators/freelist.hpp
  include/alloy/extra/allocators/memory_arena_cache.hpp
)

set(source_files
  src/alloy/extra/allocators/memory_arena_cache.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}ExtraAllocators)
add_library(${PROJECT_NAME}::Extra::Allocators ALIAS
  ${PROJECT_NAME}ExtraAllocators
)

target_sources(${PROJECT_NAME}ExtraAllocators
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include
      FILES ${header_files}
  PRIVATE
    ${source_files}
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
    FOLDER "Optional Component Targets"
)

# Header Self-Containment Tests

include(SurrogateTarget)

if (ALLOY_COMPILE_SURROGATE_TESTS)
  add_surrogate_target(${PROJECT_NAME}ExtraAllocators.Surrogate
    TARGET ${PROJECT_NAME}ExtraAllocators
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  # add_subdirectory("example")
endif ()
