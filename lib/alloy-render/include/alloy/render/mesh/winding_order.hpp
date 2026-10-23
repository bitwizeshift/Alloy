// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file winding_order.hpp
///
/// \brief This header defines the vertex orders that identify the front face
///        of a primitive
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_WINDING_ORDER_HPP
#define ALLOY_RENDER_MESH_WINDING_ORDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/string/zstring_view.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The order in which the vertices of a primitive appear when
  ///        viewing its front face
  //////////////////////////////////////////////////////////////////////////////
  enum class winding_order {
    /// Front faces list their vertices counter-clockwise (default)
    ///
    /// Corresponds to GL_CCW in OpenGL, `FrontCounterClockwise = TRUE` in
    /// DX11, etc.
    counter_clockwise,

    /// Front faces list their vertices clockwise
    ///
    /// Corresponds to GL_CW in OpenGL, `FrontCounterClockwise = FALSE` in
    /// DX11, etc.
    clockwise,
  };

  /// \brief Converts this winding order \p w into a string
  ///
  /// \warning
  /// If \p w is not a valid enumerator for `winding_order`, the behavior is
  /// undefined!
  ///
  /// \param w the winding_order to convert to string
  /// \return a null-terminated string
  auto to_string(winding_order w) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(winding_order w) noexcept -> core::zstring_view
{
  switch (w) {
    case winding_order::counter_clockwise: {
      return "counter_clockwise";
    }
    case winding_order::clockwise: {
      return "clockwise";
    }
  }
  core::compiler::unreachable();
}
#endif /* ALLOY_RENDER_MESH_WINDING_ORDER_HPP */
