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
add_alloy_library(${PROJECT_NAME}Engine
  ALIAS             ${PROJECT_NAME}::Engine
  OUTPUT_NAME       alloy-engine
  EXPORT_NAME       Engine
  HEADERS           ${header_files}
  GENERATED_HEADERS ${generated_header_files}
  SOURCES           ${source_files}
  DEPENDENCIES
    PUBLIC  ${PROJECT_NAME}::Core ${PROJECT_NAME}::IO
    PRIVATE tga-lib
)

target_include_directories(${PROJECT_NAME}Engine
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party>
)

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
