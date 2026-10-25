// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/device.hpp"

#include "alloy/render/error.hpp"

#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/model/model_spec.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include "alloy/core/utilities/result.hpp"

#include <utility> // std::move

//==============================================================================
// class : device
//==============================================================================

//------------------------------------------------------------------------------
// Resource Creation
//------------------------------------------------------------------------------

auto alloy::render::device::create_program(const program_spec& spec)
  -> core::result<unique_program, render_error>
{
  auto layout = program_layout::from_spec(spec);
  if (!layout) {
    return core::fail(std::move(layout).error());
  }

  auto handle = do_create_program(spec, std::move(layout).value());
  if (!handle) {
    return core::fail(std::move(handle).error());
  }
  return unique_program{this, *handle};
}

auto alloy::render::device::create_texture(const texture_spec& spec)
  -> core::result<unique_texture, render_error>
{
  auto handle = do_create_texture(spec);
  if (!handle) {
    return core::fail(std::move(handle).error());
  }
  return unique_texture{this, *handle};
}

auto alloy::render::device::create_material(const material_spec& spec)
  -> core::result<unique_material, render_error>
{
  auto handle = do_create_material(spec);
  if (!handle) {
    return core::fail(std::move(handle).error());
  }
  return unique_material{this, *handle};
}

auto alloy::render::device::create_mesh(const mesh& mesh)
  -> core::result<unique_mesh, render_error>
{
  auto handle = do_create_mesh(mesh);
  if (!handle) {
    return core::fail(std::move(handle).error());
  }
  return unique_mesh{this, *handle};
}

auto alloy::render::device::create_model(const model_spec& spec)
  -> core::result<unique_model, render_error>
{
  auto handle = do_create_model(spec);
  if (!handle) {
    return core::fail(std::move(handle).error());
  }
  return unique_model{this, *handle};
}
