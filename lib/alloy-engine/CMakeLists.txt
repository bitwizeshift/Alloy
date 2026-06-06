##############################################################################
# Configured Files
##############################################################################

# read license, indent each line by 2 spaces
file(READ "${PROJECT_SOURCE_DIR}/LICENSE" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

set(ALLOY_ENGINE_ENTITY_TYPE ${ALLOY_ENTITY_TYPE})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/engine/config.hpp"
  @ONLY
)
unset(ALLOY_ENGINE_ENTITY_TYPE)

##############################################################################
# Target
##############################################################################

set(generated_header_files
  ${CMAKE_CURRENT_BINARY_DIR}/include/alloy/engine/config.hpp
)
source_group(
  TREE "${CMAKE_CURRENT_BINARY_DIR}/include"
  PREFIX "Generated Header Files"
  FILES ${generated_header_files}
)

set(header_files
  include/alloy/engine/component.hpp
  include/alloy/engine/entity.hpp
  include/alloy/engine/signature.hpp
  include/alloy/engine/entity_manager.hpp
  include/alloy/engine/component_manager.hpp
  include/alloy/engine/frame_clock.hpp
  include/alloy/engine/game-loop/semi_fixed_game_loop.hpp
  include/alloy/engine/game-loop/fixed_game_loop.hpp
  include/alloy/engine/image/image_loader.hpp
  include/alloy/engine/image/tga_image_loader.hpp
  include/alloy/engine/image/image_manager.hpp
)

set(source_files
  src/alloy/engine/entity_manager.cpp
  src/alloy/engine/component_manager.cpp
  src/alloy/engine/image/tga_image_loader.cpp
  src/alloy/engine/image/image_manager.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}Engine)
add_library(${PROJECT_NAME}::Engine ALIAS ${PROJECT_NAME}Engine)

target_sources(${PROJECT_NAME}Engine
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include
      FILES ${header_files}
    FILE_SET generated TYPE HEADERS
      BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include"
      FILES ${generated_header_files}
  PRIVATE
    ${source_files}
)

target_compile_features(${PROJECT_NAME}Engine
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}Engine
  PROPERTIES
    OUTPUT_NAME "alloy-engine"
    EXPORT_NAME "Engine"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Component Targets"
)

target_include_directories(${PROJECT_NAME}Engine
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party>
)

target_link_libraries(${PROJECT_NAME}Engine
  PUBLIC ${PROJECT_NAME}::Core
  PUBLIC ${PROJECT_NAME}::IO
  PRIVATE tga-lib
)

# Header Self-Containment Tests

include(SurrogateTarget)

if (ALLOY_COMPILE_SURROGATE_TESTS)
  add_surrogate_target(${PROJECT_NAME}Engine.Surrogate
    TARGET ${PROJECT_NAME}Engine
  )
endif ()

# Unit Tests

# if (ALLOY_COMPILE_TESTS)
#   add_subdirectory("test")
# endif ()

##############################################################################
# Installation
##############################################################################

# Installation is currently disabled: exporting AlloyEngine pulls the vendored
# tga-lib into the AlloyTargets export set, which is not yet packaged. The
# install rules below are kept modern and ready for when that is resolved.
return ()

include(GNUInstallDirs)

install(
  TARGETS "${PROJECT_NAME}Engine"
  EXPORT AlloyTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILE_SET generated DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
