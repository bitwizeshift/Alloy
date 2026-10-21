// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/io/filesystem/filesystem_monitor.hpp"

//==============================================================================
// class : filesystem_monitor
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::io::filesystem_monitor::watch(std::string_view path, bool recursive)
  noexcept -> void
{
  do_watch(path, recursive);
}
