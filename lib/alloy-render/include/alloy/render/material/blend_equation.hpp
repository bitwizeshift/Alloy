////////////////////////////////////////////////////////////////////////////////
/// \file blend_equation.hpp
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

#ifndef ALLOY_RENDER_MATERIAL_BLEND_EQUATION_HPP
#define ALLOY_RENDER_MATERIAL_BLEND_EQUATION_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

namespace alloy::render {


  ////////////////////////////////////////////////////////////////////////////
  /// \brief Possible blend equations to use for this material
  ////////////////////////////////////////////////////////////////////////////
  enum class blend_equation {
    add,             ///< Additive blending. Corresponds to GL_FUNC_ADD in
                     ///< OpenGL, D3D11_BLEND_OP_ADD in DX11, etc.

    subtract,        ///< Subtractive blending. Corresponds to GL_FUNC_SUBTRACT
                     ///< in OpenGL, D3D11_BLEND_OP_SUBTRACT in DX11, etc.

    reverse_subtract,///< Reverse subtraction blending. Corresponds to
                     ///< GL_FUNC_REVERSE_SUBTRACT in OpenGL,
                     ///< D3D11_BLEND_OP_REV_SUBTRACT in DX11, etc.

    max,             ///< Takes the max component during blending. Corresponds
                     ///< to GL_FUNC_MAX in OpenGl, D3D11_BLEND_OP_MAX in DX11

    min,             ///< Takes the min component during blending. Corresponds
                     ///< to GL_FUNC_MIN in OpenGl, D3D11_BLEND_OP_MIN in DX11
  };

  /// \brief Converts this blend equation \p e into a string
  ///
  /// \warning
  /// If \p e is not a valid enumerator for `blend_equation`, the behavior is
  /// undefined!
  ///
  /// \param e the blend_equation to convert to string
  /// \return a null-terminated string
  auto to_string(blend_equation e) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(blend_equation e)
  noexcept -> core::zstring_view
{
  switch (e) {
    case blend_equation::add: {
      return "add";
    }
    case blend_equation::subtract: {
      return "subtract";
    }
    case blend_equation::reverse_subtract: {
      return "reverse_subtract";
    }
    case blend_equation::max: {
      return "max";
    }
    case blend_equation::min: {
      return "min";
    }
  }
  ALLOY_ASSERT(false, "This is unreachable");
  core::compiler::unreachable();
}


#endif /* ALLOY_RENDER_MATERIAL_BLEND_EQUATION_HPP */
