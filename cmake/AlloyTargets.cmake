# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# AlloyTargets
# ------------
#
# The family of helpers used to declare Alloy's component libraries, unit-test
# runners and benchmark runners. Each leaf ``CMakeLists.txt`` keeps its own
# itemized source/header lists and any ``configure_file`` preamble, then defines
# the target with a single declarative call so the near-identical boilerplate is
# not copy-pasted across every component.

include(SurrogateTarget)

# Internal: link a target against the per-visibility list of dependencies parsed
# out of a nested ``DEPENDENCIES PUBLIC ... PRIVATE ... INTERFACE ...`` argument.
function(_alloy_link_dependencies target)
  cmake_parse_arguments(dep "" "" "PUBLIC;PRIVATE;INTERFACE" ${ARGN})
  if (dep_PUBLIC)
    target_link_libraries("${target}" PUBLIC ${dep_PUBLIC})
  endif ()
  if (dep_PRIVATE)
    target_link_libraries("${target}" PRIVATE ${dep_PRIVATE})
  endif ()
  if (dep_INTERFACE)
    target_link_libraries("${target}" INTERFACE ${dep_INTERFACE})
  endif ()
endfunction()

#.rst:
# .. command:: add_alloy_library
#
#   Declares an Alloy component library and everything that uniformly goes with
#   it: the namespaced ALIAS, IDE source groups, the public header file-sets,
#   the C++17 requirement, the standard target properties, the import/export
#   definitions, the link against ``Alloy::CommonFlags`` and the header
#   self-containment (surrogate) target.
#
#   ::
#
#       add_alloy_library(<name>
#         ALIAS              <alias>
#         OUTPUT_NAME        <name>
#         [EXPORT_NAME       <name>]
#         [API_PREFIX        <PREFIX>]
#         [FOLDER            <folder>]      # default: "Component Targets"
#         [STATIC]                          # force a static library
#         [HIDDEN_VISIBILITY]               # hide non-API symbols (decorated libs)
#         [HEADERS           <header>...]
#         [GENERATED_HEADERS <header>...]
#         [SOURCES           <source>...]
#         [DEPENDENCIES PUBLIC <lib>... PRIVATE <lib>... INTERFACE <lib>...]
#       )
#
function(add_alloy_library name)
  cmake_parse_arguments(arg
    "STATIC;HIDDEN_VISIBILITY"
    "ALIAS;OUTPUT_NAME;EXPORT_NAME;API_PREFIX;FOLDER"
    "HEADERS;GENERATED_HEADERS;SOURCES;DEPENDENCIES"
    ${ARGN}
  )

  if (NOT arg_FOLDER)
    set(arg_FOLDER "Component Targets")
  endif ()

  ############################### Create target ##############################

  if (arg_STATIC)
    add_library("${name}" STATIC)
  else ()
    add_library("${name}")
  endif ()
  if (arg_ALIAS)
    add_library("${arg_ALIAS}" ALIAS "${name}")
  endif ()

  ############################## Source groups ###############################

  if (arg_SOURCES)
    source_group(
      TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
      PREFIX "Source Files"
      FILES ${arg_SOURCES}
    )
  endif ()
  if (arg_HEADERS)
    source_group(
      TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
      PREFIX "Header Files"
      FILES ${arg_HEADERS}
    )
  endif ()
  if (arg_GENERATED_HEADERS)
    source_group(
      TREE "${CMAKE_CURRENT_BINARY_DIR}/include"
      PREFIX "Generated Header Files"
      FILES ${arg_GENERATED_HEADERS}
    )
  endif ()

  ################################ Sources ###################################

  if (arg_HEADERS)
    target_sources("${name}"
      PUBLIC
        FILE_SET HEADERS
          BASE_DIRS include
          FILES ${arg_HEADERS}
    )
  endif ()
  if (arg_GENERATED_HEADERS)
    target_sources("${name}"
      PUBLIC
        FILE_SET generated TYPE HEADERS
          BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include"
          FILES ${arg_GENERATED_HEADERS}
    )
  endif ()
  if (arg_SOURCES)
    target_sources("${name}" PRIVATE ${arg_SOURCES})
  endif ()

  ############################## Properties ##################################

  target_compile_features("${name}" PUBLIC cxx_std_20)
  set_target_properties("${name}"
    PROPERTIES
      OUTPUT_NAME "${arg_OUTPUT_NAME}"
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
      FOLDER "${arg_FOLDER}"
  )
  if (arg_EXPORT_NAME)
    set_target_properties("${name}" PROPERTIES EXPORT_NAME "${arg_EXPORT_NAME}")
  endif ()

  # Only libraries that actually decorate their public API with the export macro
  # may hide their remaining symbols; doing so for an undecorated library would
  # make every symbol local and break linking against it in shared builds.
  if (arg_HIDDEN_VISIBILITY)
    set_target_properties("${name}"
      PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
  endif ()

  ########################### Import/export macros ###########################

  if (arg_API_PREFIX)
    if (BUILD_SHARED_LIBS AND NOT arg_STATIC)
      target_compile_definitions("${name}" PRIVATE ${arg_API_PREFIX}_EXPORT=1)
    else ()
      target_compile_definitions("${name}" PUBLIC ${arg_API_PREFIX}_STATIC=1)
    endif ()
  endif ()

  ################################ Linking ###################################

  target_link_libraries("${name}" PRIVATE ${PROJECT_NAME}::CommonFlags)
  _alloy_link_dependencies("${name}" ${arg_DEPENDENCIES})

  ####################### Header self-containment test #######################

  if (arg_HEADERS AND ALLOY_COMPILE_SURROGATE_TESTS)
    add_surrogate_target("${name}.Surrogate" TARGET "${name}")
    # CommonFlags is linked PRIVATE above, so it does not propagate to the
    # surrogate; apply it directly so headers are checked under the same flags.
    target_link_libraries("${name}.Surrogate" PRIVATE ${PROJECT_NAME}::CommonFlags)
  endif ()
endfunction()

#.rst:
# .. command:: add_alloy_test
#
#   Declares a Catch2-based unit-test runner: source groups, the executable
#   (marked for coverage discovery), the namespaced ALIAS, standard test
#   properties, the link against Catch2/the component/``Alloy::CommonFlags``, and
#   CTest registration via ``catch_discover_tests``.
#
#   ::
#
#       add_alloy_test(<name>
#         ALIAS             <alias>
#         OUTPUT_NAME       <name>
#         SOURCES           <source>...
#         [LIBRARIES        <lib>...]          # component(s) under test
#         [WORKING_DIRECTORY <dir>]
#       )
#
function(add_alloy_test name)
  cmake_parse_arguments(arg
    ""
    "ALIAS;OUTPUT_NAME;WORKING_DIRECTORY"
    "SOURCES;LIBRARIES"
    ${ARGN}
  )

  if (arg_SOURCES)
    source_group(
      TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
      PREFIX "Source Files"
      FILES ${arg_SOURCES}
    )
  endif ()

  add_executable("${name}" ${arg_SOURCES})
  set_property(TARGET "${name}" PROPERTY ALLOY_TEST_EXECUTABLE TRUE)
  set_property(GLOBAL APPEND PROPERTY ALLOY_TEST_EXECUTABLES "${name}")
  if (arg_ALIAS)
    add_executable("${arg_ALIAS}" ALIAS "${name}")
  endif ()

  set_target_properties("${name}"
    PROPERTIES
      OUTPUT_NAME "${arg_OUTPUT_NAME}"
      FOLDER "Unit Test Targets"
  )

  target_link_libraries("${name}"
    PRIVATE
      Catch2::Catch2WithMain
      ${PROJECT_NAME}::CommonFlags
      ${arg_LIBRARIES}
  )

  include(Catch)
  if (arg_WORKING_DIRECTORY)
    catch_discover_tests("${name}" WORKING_DIRECTORY "${arg_WORKING_DIRECTORY}")
  else ()
    catch_discover_tests("${name}")
  endif ()
endfunction()

#.rst:
# .. command:: add_alloy_benchmark
#
#   Declares a Catch2-based benchmark runner and marks it so the ``benchmark``
#   target (see ``enable_benchmark_target``) can discover it. Mirrors
#   ``add_alloy_test`` but is not registered with CTest.
#
#   ::
#
#       add_alloy_benchmark(<name>
#         ALIAS       <alias>
#         OUTPUT_NAME <name>
#         SOURCES     <source>...
#         [LIBRARIES  <lib>...]
#       )
#
function(add_alloy_benchmark name)
  cmake_parse_arguments(arg
    ""
    "ALIAS;OUTPUT_NAME"
    "SOURCES;LIBRARIES"
    ${ARGN}
  )

  if (arg_SOURCES)
    source_group(
      TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
      PREFIX "Source Files"
      FILES ${arg_SOURCES}
    )
  endif ()

  add_executable("${name}" ${arg_SOURCES})
  set_property(TARGET "${name}" PROPERTY ALLOY_BENCHMARK_EXECUTABLE TRUE)
  set_property(GLOBAL APPEND PROPERTY ALLOY_BENCHMARK_EXECUTABLES "${name}")
  if (arg_ALIAS)
    add_executable("${arg_ALIAS}" ALIAS "${name}")
  endif ()

  set_target_properties("${name}"
    PROPERTIES
      OUTPUT_NAME "${arg_OUTPUT_NAME}"
      FOLDER "Benchmark Targets"
  )

  target_link_libraries("${name}"
    PRIVATE
      Catch2::Catch2WithMain
      ${PROJECT_NAME}::CommonFlags
      ${arg_LIBRARIES}
  )
endfunction()

#.rst:
# .. command:: enable_benchmark_target
#
#   Defines the ``benchmark`` target, which runs every declared benchmark
#   executable sequentially. Excluded from the default build; runs only when
#   invoked explicitly (``cmake --build ... --target benchmark``). Call once,
#   after all benchmark executables have been declared.
#
#   ::
#
#       enable_benchmark_target()
#
function(enable_benchmark_target)
  get_property(_benchmarks GLOBAL PROPERTY ALLOY_BENCHMARK_EXECUTABLES)
  if (NOT _benchmarks)
    return ()
  endif ()

  set(_commands "")
  foreach (_bench IN LISTS _benchmarks)
    list(APPEND _commands COMMAND "$<TARGET_FILE:${_bench}>")
  endforeach ()

  add_custom_target(benchmark
    ${_commands}
    DEPENDS ${_benchmarks}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Running benchmarks"
    USES_TERMINAL
    VERBATIM
  )
endfunction()
