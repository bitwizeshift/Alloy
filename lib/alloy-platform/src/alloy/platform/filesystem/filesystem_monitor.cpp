// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/platform/filesystem/filesystem_monitor.hpp"
#include <string_view>

//==============================================================================
// class : filesystem_monitor
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::platform::filesystem_monitor::watch(std::string_view path, bool recursive)
  noexcept -> void
{
  do_watch(path, recursive);
}
