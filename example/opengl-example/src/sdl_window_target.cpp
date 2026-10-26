// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "sdl_window_target.hpp"

#include <alloy/core/utilities/not_null.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_gl_window.hpp>
#include <alloy/render/gl/surface/presentation_target.hpp>

#include <cstdint> // std::uint32_t

//==============================================================================
// class : sdl_window_target
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

alloy::example::sdl_window_target::sdl_window_target(
  core::not_null<extra::sdl2_gl_window*> window
) noexcept
  : m_window{window}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::example::sdl_window_target::drawable_extent()
  const -> render::gl::surface_extent
{
  const auto dimensions = m_window->drawable_dimensions();
  return render::gl::surface_extent{
    .width = static_cast<std::uint32_t>(dimensions.width),
    .height = static_cast<std::uint32_t>(dimensions.height),
  };
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::example::sdl_window_target::present() -> void
{
  // Updating an SDL GL window swaps its buffers
  m_window->update();
}
