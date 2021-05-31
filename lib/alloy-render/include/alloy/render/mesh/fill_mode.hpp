////////////////////////////////////////////////////////////////////////////////
/// \file fill_mode.hpp
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

#ifndef ALLOY_RENDER_MESH_FILL_MODE_HPP
#define ALLOY_RENDER_MESH_FILL_MODE_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The fill mode for a given mesh
  //////////////////////////////////////////////////////////////////////////////
  enum class fill_mode {
    /// Renders meshes solid (default)
    ///
    /// Corresponds to GL_FILL in OpenGL, D3D11_FILL_SOLID in DX11, etc.
    solid,

    /// Renders meshes as a wireframe without any fill
    ///
    /// Corresponds to GL_LINE in OpenGL, D3D11_FILL_WIREFRAME in DX11, etc.
    wireframe,
  };

  /// \brief Converts this fill mode \p m into a string
  ///
  /// \warning
  /// If \p m is not a valid enumerator for `fill_mode`, the behavior is
  /// undefined!
  ///
  /// \param m the fill_mode to convert to string
  /// \return a null-terminated string
  auto to_string(fill_mode m) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(fill_mode m)
  noexcept -> core::zstring_view
{
  switch (m) {
    case fill_mode::solid: {
      return "solid";
    }
    case fill_mode::wireframe: {
      return "wireframe";
    }
  }
  core::compiler::unreachable();
}
#endif /* ALLOY_RENDER_MESH_FILL_MODE_HPP */
