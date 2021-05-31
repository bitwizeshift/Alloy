////////////////////////////////////////////////////////////////////////////////
/// \file depth_compare.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ALLOY_RENDER_MATERIAL_DEPTH_COMPARE_HPP
#define ALLOY_RENDER_MATERIAL_DEPTH_COMPARE_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

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
auto alloy::render::to_string(depth_compare d)
  noexcept -> core::zstring_view
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
