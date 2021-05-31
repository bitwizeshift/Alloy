////////////////////////////////////////////////////////////////////////////////
/// \file sphere_mesh_builder.hpp
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

#ifndef ALLOY_RENDER_MESH_3D_SPHERE_MESH_BUILDER_HPP
#define ALLOY_RENDER_MESH_3D_SPHERE_MESH_BUILDER_HPP

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"

#include "alloy/core/memory/allocator.hpp"

#include <cstddef> // std::size_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A simple builder class for constructing 3d meshes of spheres
  //////////////////////////////////////////////////////////////////////////////
  class sphere_builder final : public mesh_builder
  {
    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this
    sphere_builder(std::size_t x_segments, std::size_t y_segments) noexcept;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    auto build_with(core::allocator alloc) const -> mesh override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::size_t m_x_segments;
    std::size_t m_y_segments;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_3D_SPHERE_MESH_BUILDER_HPP */
