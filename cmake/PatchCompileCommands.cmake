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

#[[
  The MIT License (MIT)

  Copyright (c) 2022 Matthew Rodusek

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
]]

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