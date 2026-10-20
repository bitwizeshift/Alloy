#[[
  Copyright (c) Matthew Rodusek
  SPDX-License-Identifier: MIT
  See the LICENSE file at the root of this repository for the full text.
]]

# Patch compile commands
# ======================
#
# A simple module for patchin the compile_commands.json to be in the `build`
# directory rather than a sub-directory. This only occurs if `MSL_CONFIGURE_PRESET`
# is present
#
# This patching primarily occurs for IDEs that look for the compile_commands.json
# file in a static place like `build`, such as VSCode, so that it's in an easy
# to discover location.

# Don't patch unless we say to
if (NOT ALLOY_PATCH_COMPILE_COMMANDS)
  return ()
endif ()

# Don't patch if our build directory isn't actually under build
if (NOT CMAKE_BINARY_DIR MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/build/.*")
  return ()
endif ()

# If the file already exists, don't repatch it.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/compile_commands.json")
  return ()
endif ()

file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/build/compile_commands.json"
  RESULT status
  SYMBOLIC
)

if (NOT status EQUAL 0)
  message(WARNING "Unable to link compile_commands.json file")
endif ()