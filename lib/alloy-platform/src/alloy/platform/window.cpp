// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/platform/window.hpp"

#include "alloy/core/assert.hpp"

//==============================================================================
// class : window
//==============================================================================

//------------------------------------------------------------------------------
// Destructors
//------------------------------------------------------------------------------

alloy::platform::window::~window() noexcept = default;

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::platform::window::width() const -> int
{
  return do_width();
}

auto alloy::platform::window::height() const -> int
{
  return do_height();
}

auto alloy::platform::window::is_fullscreen() const -> bool
{
  return do_is_fullscreen();
}

auto alloy::platform::window::is_focused() const -> bool
{
  return do_is_focused();
}

auto alloy::platform::window::is_hidden() const -> bool
{
  return do_is_hidden();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::platform::window::update() -> void
{
  do_update();
}

auto alloy::platform::window::set_dimensions(int width, int height) -> void
{
  ALLOY_ASSERT( width > 0 );
  ALLOY_ASSERT( height > 0 );

  do_set_dimensions(width,height);
}

auto alloy::platform::window::set_position(int x, int y) -> void
{
  do_set_position(x,y);
}

//------------------------------------------------------------------------------
// Access
//------------------------------------------------------------------------------

auto alloy::platform::window::fullscreen() -> void
{
  do_fullscreen();
}

auto alloy::platform::window::focus() -> void
{
  do_focus();
}

auto alloy::platform::window::show() -> void
{
  do_show();
}

auto alloy::platform::window::hide() -> void
{
  do_hide();
}
