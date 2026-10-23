// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file cube_builder.hpp
///
/// \brief This header defines a builder for cube meshes
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_3D_CUBE_BUILDER_HPP
#define ALLOY_RENDER_MESH_3D_CUBE_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"

#include "alloy/core/memory/allocator.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A simple builder class for constructing 3d meshes of cubes
  ///
  /// The cube spans `[-1, 1]` on every axis, and has 3-component positions,
  /// 2-component UVs, and 3-component normals. Each face has its own 4
  /// vertices, so that its normals and UVs are not shared with neighbouring
  /// faces, and is wound counter-clockwise when viewed from outside the cube.
  ///
  /// The UVs map the faces onto a texture divided into a 2 by 3 grid, with
  /// the bottom and back faces in the first row, the top and front faces in
  /// the second, and the left and right faces in the third.
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

    /// \brief Builds the cube using memory from \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the mesh object
    auto build_with(core::allocator alloc) const -> mesh override;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_3D_CUBE_BUILDER_HPP */
