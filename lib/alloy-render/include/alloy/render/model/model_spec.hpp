// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file model_spec.hpp
///
/// \brief This header defines the description of a model that a render
///        device creates
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MODEL_MODEL_SPEC_HPP
#define ALLOY_RENDER_MODEL_MODEL_SPEC_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/resource/resource_handle.hpp"

#include <span> // std::span

namespace alloy::render {

  struct model_tag;

  /// \brief A non-owning handle to a model created by a device
  using model_handle = resource_handle<model_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of a model to be created by a device
  ///
  /// A model is a mesh paired with the material that draws each of its
  /// submeshes. Every material's program must read only vertex inputs that
  /// the mesh provides, and every program must declare the same instance
  /// inputs, so that a single set of instance values can draw the whole
  /// model.
  ///
  /// The spec only views its materials, which must outlive the call that
  /// creates the model.
  //////////////////////////////////////////////////////////////////////////////
  struct model_spec
  {
    /// The mesh that the model draws, which must outlive the model
    mesh_handle mesh;

    /// The material that draws each submesh of the mesh, in submesh order.
    /// Each material must outlive the model.
    std::span<const material_handle> materials;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MODEL_MODEL_SPEC_HPP */
