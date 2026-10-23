// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file sphere_builder.hpp
///
/// \brief This header defines a builder for UV-sphere meshes
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_3D_SPHERE_BUILDER_HPP
#define ALLOY_RENDER_MESH_3D_SPHERE_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"

#include "alloy/core/memory/allocator.hpp"

#include <cstddef> // std::size_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A simple builder class for constructing 3d meshes of spheres
  ///
  /// The sphere is a UV-sphere with a radius of `1`, centered at the origin,
  /// with 3-component positions, 2-component UVs, and 3-component normals. It
  /// is divided into `x_segments` around its equator and `y_segments` from
  /// pole to pole, and is wound counter-clockwise when viewed from outside.
  ///
  /// The UVs map longitude to `u` and latitude to `v`, from `0` to `1`. A
  /// seam of duplicated vertices is placed where `u` wraps around.
  //////////////////////////////////////////////////////////////////////////////
  class sphere_builder final : public mesh_builder
  {
    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a builder for spheres with the specified number of
    ///        segments
    ///
    /// \pre `x_segments >= 3`
    /// \pre `y_segments >= 2`
    /// \param x_segments the number of segments around the equator
    /// \param y_segments the number of segments from pole to pole
    sphere_builder(std::size_t x_segments, std::size_t y_segments) noexcept;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    /// \brief Builds the sphere using memory from \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the mesh object
    auto build_with(core::allocator alloc) const -> mesh override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::size_t m_x_segments;
    std::size_t m_y_segments;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_3D_SPHERE_BUILDER_HPP */
