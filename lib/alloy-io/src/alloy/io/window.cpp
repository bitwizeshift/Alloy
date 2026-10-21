// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/io/window.hpp"

#include "alloy/core/assert.hpp"

//==============================================================================
// class : window
//==============================================================================

//------------------------------------------------------------------------------
// Destructors
//------------------------------------------------------------------------------

alloy::io::window::~window() noexcept = default;

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::io::window::width() const -> int
{
  return do_width();
}

auto alloy::io::window::height() const -> int
{
  return do_height();
}

auto alloy::io::window::is_fullscreen() const -> bool
{
  return do_is_fullscreen();
}

auto alloy::io::window::is_focused() const -> bool
{
  return do_is_focused();
}

auto alloy::io::window::is_hidden() const -> bool
{
  return do_is_hidden();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::io::window::update() -> void
{
  do_update();
}

auto alloy::io::window::set_dimensions(int width, int height) -> void
{
  ALLOY_ASSERT( width > 0 );
  ALLOY_ASSERT( height > 0 );

  do_set_dimensions(width,height);
}

auto alloy::io::window::set_position(int x, int y) -> void
{
  do_set_position(x,y);
}

//------------------------------------------------------------------------------
// Access
//------------------------------------------------------------------------------

auto alloy::io::window::fullscreen() -> void
{
  do_fullscreen();
}

auto alloy::io::window::focus() -> void
{
  do_focus();
}

auto alloy::io::window::show() -> void
{
  do_show();
}

auto alloy::io::window::hide() -> void
{
  do_hide();
}
