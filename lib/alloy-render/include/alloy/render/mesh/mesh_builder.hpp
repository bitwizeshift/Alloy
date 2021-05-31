////////////////////////////////////////////////////////////////////////////////
/// \file mesh_builder.hpp
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

#ifndef ALLOY_RENDER_MESH_MESH_BUILDER_HPP
#define ALLOY_RENDER_MESH_MESH_BUILDER_HPP

#include "alloy/render/mesh/mesh.hpp"

#include "alloy/core/memory/allocator.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A simple abstraction for factory creation of mesh objects.
  ///
  /// In general, it's not likely that this interface will need to be used in
  /// practice, so the cost of the V-table will be negligible. However, during
  /// debugging and development, it can sometimes be useful to use an
  /// abstraction like this, since meshes can be easily "spawned" for different
  /// purposes -- such as testing material, lighting, etc.
  //////////////////////////////////////////////////////////////////////////////
  class mesh_builder
  {
    //--------------------------------------------------------------------------
    // Destructor
    //--------------------------------------------------------------------------
  public:

    virtual ~mesh_builder() = default;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    /// \brief Builds the mesh
    ///
    /// \return the mesh object
    virtual auto build() const -> mesh;

    // NOTE(bitwizeshift):
    //   This is a bit of a weak abstraction since there is no real guarantee
    //   that the allocator is used by the derived class. Not very SOLID;
    //   could this be improved by somehow forcing the allocator that will be
    //   used during the model building?

    /// \brief Builds the mesh by requesting to use memory from \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the mesh object
    virtual auto build_with(core::allocator alloc) const -> mesh = 0;
  };

} // namespace alloy::render

inline
auto alloy::render::mesh_builder::build()
  const -> mesh
{
  return build_with(core::allocator{});
}

#endif /* ALLOY_RENDER_MESH_MESH_BUILDER_HPP */
