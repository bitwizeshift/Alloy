// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file mesh_builder.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_MESH_BUILDER_HPP
#define ALLOY_RENDER_MESH_MESH_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

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
auto alloy::render::mesh_builder::build() const -> mesh
{
  return build_with(core::allocator{});
}

#endif /* ALLOY_RENDER_MESH_MESH_BUILDER_HPP */
