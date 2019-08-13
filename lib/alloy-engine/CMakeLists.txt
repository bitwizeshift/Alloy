##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/engine/component.hpp
  include/alloy/engine/entity.hpp
  include/alloy/engine/signature.hpp
  include/alloy/engine/entity_manager.hpp
  include/alloy/engine/component_manager.hpp
)

set(source_files
  src/alloy/engine/entity_manager.cpp
  src/alloy/engine/component_manager.cpp
)

add_library(${PROJECT_NAME}Engine
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Engine ALIAS ${PROJECT_NAME}Engine)

target_compile_features(${PROJECT_NAME}Engine
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}Engine
  PROPERTIES
    OUTPUT_NAME "alloy-engine"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME}Engine
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}Engine
  PUBLIC ${PROJECT_NAME}::Core
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}Engine.SelfContainmentTest
    TARGET ${PROJECT_NAME}Engine
    HEADERS ${header_files}
  )
endif ()

# Unit Tests

# if (ALLOY_COMPILE_TESTS)
#   add_subdirectory("test")
# endif ()

##############################################################################
# Installation
##############################################################################

if (ALLOY_IS_SUBPROJECT)
  return()
endif ()

include(GNUInstallDirs)

set(ALLOY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Targets
install(
  TARGETS "${PROJECT_NAME}Engine"
  EXPORT "${PROJECT_NAME}EngineTargets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}EngineTargets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${ALLOY_CMAKE_CONFIG_DESTINATION}"
)

# Includes
install(
  DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(
  DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
