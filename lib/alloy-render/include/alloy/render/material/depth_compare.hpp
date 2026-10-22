// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file depth_compare.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MATERIAL_DEPTH_COMPARE_HPP
#define ALLOY_RENDER_MATERIAL_DEPTH_COMPARE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The depth comparison function to use
  //////////////////////////////////////////////////////////////////////////////
  enum class depth_compare {

    /// Depth never passes.
    ///
    /// Corresponds to GL_NEVER in OpenGL, D3D11_COMPARISON_NEVER in DX11, etc.
    never,

    /// Passes if the incoming depth value is equal to the stored depth value.
    ///
    /// Corresponds to GL_EQUAL in OpenGL, D3D11_COMPARISON_EQUAL in DX11, etc.
    less,

    /// Passes if the incoming depth value is less than the stored depth value.
    ///
    /// Corresponds to GL_LESS in OpenGL, D3D11_COMPARISON_LESS in DX11, etc.
    equal,

    /// Passes if the incoming depth value is less than or equal to the stored
    /// depth value.
    ///
    /// Corresponds to GL_LEQUAL in OpenGL,
    /// D3D11_COMPARISON_LESS_EQUAL in DX11, etc.
    less_equal,

    /// Passes if the incoming depth value is greater than the stored depth
    /// value.
    ///
    /// Corresponds to GL_GREATER in OpenGL, D3D11_COMPARISON_GREATER in DX11, etc.
    greater,

    /// Passes if the incoming depth value is not equal to the stored depth
    /// value.
    ///
    /// Corresponds to GL_NOTEQUAL in OpenGL, D3D11_COMPARISON_NOT_EQUAL in
    /// DX11, etc.
    not_equal,

    /// Passes if the incoming depth value is greater than or equal to the
    /// stored depth value.
    ///
    /// Corresponds to GL_GEQUAL in OpenGL, D3D11_COMPARISON_GREATER_EQUAL in
    /// DX11, etc.
    greater_equal,

    /// Depth always passes.
    ///
    /// Corresponds to GL_ALWAYS in OpenGL, D3D11_COMPARISON_ALWAYS in
    /// DX11, etc.
    always,
  };

  /// \brief Converts this depth compare \p d into a string
  ///
  /// \warning
  /// If \p d is not a valid enumerator for `depth_compare`, the behavior is
  /// undefined!
  ///
  /// \param d the depth_compare to convert to string
  /// \return a null-terminated string
  auto to_string(depth_compare b) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(depth_compare d) noexcept -> core::zstring_view
{
  switch (d) {
    case depth_compare::never: {
      return "never";
    }
    case depth_compare::less: {
      return "less";
    }
    case depth_compare::equal: {
      return "equal";
    }
    case depth_compare::less_equal: {
      return "less_equal";
    }
    case depth_compare::greater: {
      return "greater";
    }
    case depth_compare::not_equal: {
      return "not_equal";
    }
    case depth_compare::greater_equal: {
      return "greater_equal";
    }
    case depth_compare::always: {
      return "always";
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_MATERIAL_DEPTH_COMPARE_HPP */
