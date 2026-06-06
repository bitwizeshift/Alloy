cmake_minimum_required(VERSION 4.3)

#.rst:
# SurrogateTarget
# ---------------
#
# Creates a C or C++ object library that includes each of another target's
# public header files independently to ensure that each header carries no
# expected ordering.
#
# This is used to avoid accidental dependencies based on the order of
# inclusion.
#
# The headers to test are read directly from the header file-sets of the
# referenced ``TARGET`` (see ``target_sources(... FILE_SET HEADERS ...)``), so
# they do not need to be listed a second time.
#
# ::
#
#     add_surrogate_target(
#       <name>
#       TARGET <target>
#       [EXTENSION <ext>]
#       [HEADERS <header>...]
#     )
#
#     <name>        - The name of the surrogate target to create
#     <target>      - The target whose public headers are compiled in isolation
#     [ext]         - Extension for the generated sources. Defaults to 'cpp'
#     [header]...   - Explicit headers to compile, overriding the headers read
#                     from <target>'s header file-sets
#
function(add_surrogate_target name)

  ############################### Parse arguments #############################

  cmake_parse_arguments(PARSE_ARGV 1 "SURROGATE" "" "TARGET;EXTENSION" "HEADERS")

  if (TARGET "${name}")
    message(FATAL_ERROR "add_surrogate_target: '${name}' already exists as a valid CMake target.")
  endif ()

  if (NOT SURROGATE_TARGET)
    message(FATAL_ERROR "add_surrogate_target: TARGET not specified")
  endif ()
  if (NOT TARGET "${SURROGATE_TARGET}")
    message(FATAL_ERROR "add_surrogate_target: specified TARGET '${SURROGATE_TARGET}' is not a valid CMake target")
  endif ()

  if (SURROGATE_EXTENSION)
    set(extension "${SURROGATE_EXTENSION}")
  else ()
    set(extension "cpp")
  endif ()

  ########################## Gather header inputs ############################

  # Prefer an explicit override; otherwise read the headers from every header
  # file-set registered on the target. File-set FILES are reported as absolute
  # paths.
  if (SURROGATE_HEADERS)
    set(header_files ${SURROGATE_HEADERS})
  else ()
    set(header_files)

    get_target_property(default_headers "${SURROGATE_TARGET}" HEADER_SET)
    if (default_headers)
      list(APPEND header_files ${default_headers})
    endif ()

    get_target_property(header_sets "${SURROGATE_TARGET}" HEADER_SETS)
    if (header_sets)
      foreach (set IN LISTS header_sets)
        get_target_property(set_files "${SURROGATE_TARGET}" "HEADER_SET_${set}")
        if (set_files)
          list(APPEND header_files ${set_files})
        endif ()
      endforeach ()
    endif ()

    list(REMOVE_DUPLICATES header_files)
  endif ()

  if (NOT header_files)
    message(FATAL_ERROR "add_surrogate_target: no headers found on TARGET '${SURROGATE_TARGET}'")
  endif ()

  # The base directories let us preserve a collision-free layout for the
  # generated sources, so identically-named headers in different directories
  # don't clobber one another.
  set(base_dirs)
  get_target_property(default_dirs "${SURROGATE_TARGET}" HEADER_DIRS)
  if (default_dirs)
    list(APPEND base_dirs ${default_dirs})
  endif ()
  if (header_sets)
    foreach (set IN LISTS header_sets)
      get_target_property(set_dirs "${SURROGATE_TARGET}" "HEADER_DIRS_${set}")
      if (set_dirs)
        list(APPEND base_dirs ${set_dirs})
      endif ()
    endforeach ()
  endif ()

  ############################# Generate sources #############################

  set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/${name}")
  set(surrogate_sources)

  foreach (header IN LISTS header_files)
    cmake_path(ABSOLUTE_PATH header NORMALIZE OUTPUT_VARIABLE absolute_header)

    # Find the base directory that contains this header to build a relative
    # output path; fall back to the header's own directory if none matches.
    set(relative_header "")
    foreach (base_dir IN LISTS base_dirs)
      cmake_path(IS_PREFIX base_dir "${absolute_header}" NORMALIZE is_under_base)
      if (is_under_base)
        cmake_path(RELATIVE_PATH absolute_header BASE_DIRECTORY "${base_dir}"
          OUTPUT_VARIABLE relative_header
        )
        break ()
      endif ()
    endforeach ()
    if (NOT relative_header)
      cmake_path(GET absolute_header FILENAME relative_header)
    endif ()

    set(output_path "${output_dir}/${relative_header}.${extension}")

    # file(CONFIGURE) only rewrites the file when the content changes and
    # registers it for cleanup automatically.
    file(CONFIGURE
      OUTPUT "${output_path}"
      CONTENT "#include \"@absolute_header@\" // IWYU pragma: keep\n"
      @ONLY
    )

    list(APPEND surrogate_sources "${output_path}")
  endforeach ()

  ########################### Create surrogate ##############################

  add_library("${name}" OBJECT ${surrogate_sources})
  set_target_properties("${name}"
    PROPERTIES
      FOLDER "Surrogate Tests"
  )

  # Linking the referenced target privately propagates all of its public usage
  # requirements (include directories, compile options/definitions/features and
  # link libraries) onto the surrogate. The object files are compiled but never
  # linked, which is exactly the self-containment check we want.
  target_link_libraries("${name}" PRIVATE "${SURROGATE_TARGET}")

endfunction()
