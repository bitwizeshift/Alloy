////////////////////////////////////////////////////////////////////////////////
/// \file blend_factor.hpp
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

#ifndef ALLOY_RENDER_MATERIAL_BLEND_FACTOR_HPP
#define ALLOY_RENDER_MATERIAL_BLEND_FACTOR_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Channel blending for the source and destination components
  ///
  /// These constants represent "scale factors", and determine how much of that
  /// color component will apply to the final blending
  //////////////////////////////////////////////////////////////////////////////
  enum class blend_factor {

    /// All color will be taken from the associated color component.
    ///
    /// Corresponds to GL_ONE in OpenGl, or D3D11_BLEND_ONE in DX11
    one,

    /// No color will be taken from the associated color component.
    ///
    /// Corresponds to GL_ZERO in OpenGL, or D3D11_BLEND_ZERO in DX11
    zero,

    // Source

    /// Computes a component of color from the source blend by taking all color
    /// components as a fraction of the sum of color components.
    ///
    /// Corresponds to GL_SRC_COLOR in OpenGL, or D3D11_BLEND_SRC_COLOR in DX11
    source_color,

    /// Computes a component of color from the source blend by taking only its
    /// alpha components as a fraction of the sum of alpha components.
    ///
    /// Corresponds to GL_SRC_COLOR in OpenGL, or D3D11_BLEND_SRC_COLOR in DX11
    source_alpha,

    /// The equivalent of 'one' subtracting the blend of 'source_color'.
    ///
    /// Corresponds to GL_ONE_MINUS_SRC_COLOR in OpenGL, or
    /// D3D11_BLEND_INV_SRC_COLOR in DX11
    inverse_source_color,

    /// The equivalent of 'one' subtracting the blend of 'inverse_source_color'.
    ///
    /// Corresponds to GL_ONE_MINUS_SRC_ALPHA in OpenGL, or
    /// D3D11_BLEND_INV_SRC_ALPHA in DX11
    inverse_source_alpha,

    // Destination

    /// Computes a component of color from the destination blend by taking all
    /// color components as a fraction of the sum of color components.
    ///
    /// Corresponds to GL_DST_COLOR in OpenGL, or D3D11_BLEND_DEST_COLOR in DX11
    destination_color,

    /// Computes a component of color from the destination blend by taking only
    /// its alpha components as a fraction of the sum of alpha components.
    ///
    /// Corresponds to GL_DST_COLOR in OpenGL, or D3D11_BLEND_DEST_COLOR in DX11
    destination_alpha,

    /// The equivalent of 'one' subtracting the blend of 'destination_color'.
    ///
    /// Corresponds to GL_ONE_MINUS_DST_COLOR in OpenGL, or
    /// D3D11_BLEND_INV_DEST_COLOR in DX11
    inverse_destination_color,

    /// The equivalent of 'one' subtracting the blend of 'destination_alpha'.
    ///
    /// Corresponds to GL_ONE_MINUS_DST_ALPHA in OpenGL, or
    /// D3D11_BLEND_INV_DEST_ALPHA in DX11
    inverse_destination_alpha,

    // Constant

    /// The blend color is specified as a constant value to the underlying
    /// context
    ///
    /// Corresponds to GL_CONSTANT_COLOR in OpenGL, or D3D11_BLEND_FACTOR in DX11.
    constant_color,

    /// The equivalent of 'one' subtracting the blend of `constant_color`.
    ///
    /// Corresponds to GL_ONE_MINUS_CONSTANT_COLOR in OpenGL, or
    /// D3D11_BLEND_INV_FACTOR in DX11.
    inverse_constant_color,
  };

  /// \brief Converts this blend factor \p b into a string
  ///
  /// \warning
  /// If \p b is not a valid enumerator for `blend_factor`, the behavior is
  /// undefined!
  ///
  /// \param b the blend_factor to convert to string
  /// \return a null-terminated string
  auto to_string(blend_factor b) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(blend_factor b)
  noexcept -> core::zstring_view
{
  switch (b) {
    case blend_factor::one: {
      return "one";
    }
    case blend_factor::zero: {
      return "zero";
    }
    case blend_factor::source_color: {
      return "source_color";
    }
    case blend_factor::source_alpha: {
      return "source_alpha";
    }
    case blend_factor::inverse_source_color: {
      return "inverse_source_color";
    }
    case blend_factor::inverse_source_alpha: {
      return "inverse_source_alpha";
    }
    case blend_factor::destination_color: {
      return "destination_color";
    }
    case blend_factor::destination_alpha: {
      return "destination_alpha";
    }
    case blend_factor::inverse_destination_color: {
      return "inverse_destination_color";
    }
    case blend_factor::inverse_destination_alpha: {
      return "inverse_destination_alpha";
    }
    case blend_factor::constant_color: {
      return "constant_color";
    }
    case blend_factor::inverse_constant_color: {
      return "inverse_constant_color";
    }
  }
  ALLOY_ASSERT(false, "This is unreachable");
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_MATERIAL_BLEND_FACTOR_HPP */
