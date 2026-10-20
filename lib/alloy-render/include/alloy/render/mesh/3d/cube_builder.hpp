// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file cube_mesh_builder.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_3D_CUBE_BUILDER_HPP
#define ALLOY_RENDER_MESH_3D_CUBE_BUILDER_HPP

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"

#include "alloy/core/memory/allocator.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A simple builder class for constructing 3d meshes of cubes
  //////////////////////////////////////////////////////////////////////////////
  class cube_builder final : public mesh_builder
  {
    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    cube_builder() = default;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    auto build_with(core::allocator alloc) const -> mesh;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_3D_CUBE_BUILDER_HPP */
