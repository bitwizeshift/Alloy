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
add_alloy_library(${PROJECT_NAME}ExtraAllocators
  ALIAS       ${PROJECT_NAME}::Extra::Allocators
  OUTPUT_NAME alloy-extra-allocators
  FOLDER      "Optional Component Targets"
  HEADERS     ${header_files}
  SOURCES     ${source_files}
  DEPENDENCIES
    PUBLIC ${PROJECT_NAME}::Core
)

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  # add_subdirectory("example")
endif ()
