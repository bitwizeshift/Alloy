##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/io/sdl2_window.hpp
  include/alloy/io/sdl2_gl_window.hpp
)

set(source_files
  src/alloy/io/sdl2_window.cpp
  src/alloy/io/sdl2_gl_window.cpp
)

add_library(${PROJECT_NAME}IOSDL2 STATIC
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::IO::SDL2 ALIAS ${PROJECT_NAME}IOSDL2)

target_compile_features(${PROJECT_NAME}IOSDL2
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}IOSDL2
  PROPERTIES
    OUTPUT_NAME "alloy-io-sdl2"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME}IOSDL2
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}IOSDL2
  PUBLIC SDL2
  PRIVATE ${PROJECT_NAME}::IO
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}IOSDL2.SelfContainmentTest
    TARGET ${PROJECT_NAME}IOSDL2
    HEADERS ${header_files}
  )
endif ()