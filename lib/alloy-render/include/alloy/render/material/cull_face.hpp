////////////////////////////////////////////////////////////////////////////////
/// \file cull_face.hpp
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

#ifndef ALLOY_RENDER_MATERIAL_CULL_FACE_HPP
#define ALLOY_RENDER_MATERIAL_CULL_FACE_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The face to cull when determining what is cut from the scene.
  ///
  /// What is considered the "front" or "back" face is determined by the winding
  /// order.
  //////////////////////////////////////////////////////////////////////////////
  enum class cull_face {
    /// Do not cull any triangles
    ///
    /// This corresponds to D3D11_CULL_NONE in DX11. In OpenGL, this is simply
    /// OpenGL's default mode **without any culling enabled.**
    none,

    /// Cull only the front faces as determined by the winding order
    ///
    /// This corresponds to `GL_FRONT` in OpenGL, D3D11_CULL_FRONT in DX11, etc.
    front,

    /// Cull only the back faces as determined by the winding order
    ///
    /// This corresponds to `GL_BACK` in OpenGL, D3D11_CULL_BACK in DX11, etc.
    back,
  };

  /// \brief Converts this cull face \p c into a string
  ///
  /// \warning
  /// If \p c is not a valid enumerator for `cull_face`, the behavior is
  /// undefined!
  ///
  /// \param c the cull_face to convert to string
  /// \return a null-terminated string
  auto to_string(cull_face c) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(cull_face c)
  noexcept -> core::zstring_view
{
  switch (c) {
    case cull_face::none: {
      return "none";
    }
    case cull_face::front: {
      return "front";
    }
    case cull_face::back: {
      return "back";
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_MATERIAL_CULL_FACE_HPP */
