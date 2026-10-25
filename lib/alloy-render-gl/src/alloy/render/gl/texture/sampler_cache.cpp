// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/texture/sampler_cache.hpp"

#include "alloy/render/gl/texture/sampler.hpp"

#include "alloy/render/texture/sampler_state.hpp"

#include <GL/glew.h>

//==============================================================================
// class : sampler_cache
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::sampler_cache::sampler_cache(float max_anisotropy) noexcept
  : m_samplers{},
    m_max_anisotropy{max_anisotropy}
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::render::gl::sampler_cache::find_or_create(
  const sampler_state& state
) -> ::GLuint
{
  if (const auto it = m_samplers.find(state); it != m_samplers.end()) {
    return it->second.id();
  }

  const auto [it, _] = m_samplers.emplace(
    state,
    sampler::make(state, m_max_anisotropy)
  );
  return it->second.id();
}
