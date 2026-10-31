// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file sampler_cache.hpp
///
/// \brief This header defines a cache of OpenGL sampler objects
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_TEXTURE_SAMPLER_CACHE_HPP
#define ALLOY_RENDER_GL_TEXTURE_SAMPLER_CACHE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/texture/sampler.hpp"

#include "alloy/core/containers/hash_map.hpp"

#include "alloy/render/texture/sampler_state.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A cache that shares one OpenGL sampler object between every use
  ///        of an equal sampler state
  ///
  /// Samplers are created on first use and live as long as the cache.
  //////////////////////////////////////////////////////////////////////////////
  class sampler_cache
  {
    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an empty cache whose samplers filter with at most
    ///        \p max_anisotropy
    ///
    /// \param max_anisotropy the maximum anisotropy supported by the context
    explicit sampler_cache(float max_anisotropy) noexcept;

    sampler_cache(const sampler_cache&) = delete;
    sampler_cache(sampler_cache&&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(const sampler_cache&) -> sampler_cache& = delete;
    auto operator=(sampler_cache&&) -> sampler_cache& = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Finds the sampler for \p state, creating it if this cache does
    ///        not yet hold one
    ///
    /// \param state the sampler state
    /// \return the OpenGL name of the sampler
    auto find_or_create(const sampler_state& state) -> ::GLuint;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::hash_map<sampler_state, sampler> m_samplers;
    float m_max_anisotropy;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_TEXTURE_SAMPLER_CACHE_HPP */
