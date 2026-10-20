// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file sphere_mesh_builder.hpp
///
/// \brief TODO(Bitwize): Add description
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

#endif /* ALLOY_RENDER_MESH_3D_SPHERE_BUILDER_HPP */
