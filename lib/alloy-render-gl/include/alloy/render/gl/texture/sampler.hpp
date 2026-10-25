// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file sampler.hpp
///
/// \brief This header defines the owner of an OpenGL sampler object
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_TEXTURE_SAMPLER_HPP
#define ALLOY_RENDER_GL_TEXTURE_SAMPLER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/texture/sampler_state.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of an OpenGL sampler object
  //////////////////////////////////////////////////////////////////////////////
  class sampler
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes a sampler that samples as described by \p state
    ///
    /// \param state the sampler state
    /// \param max_anisotropy the maximum anisotropy supported by the context,
    ///        which the anisotropy of \p state is clamped to
    /// \return the sampler
    static auto make(const sampler_state& state, float max_anisotropy)
      noexcept -> sampler;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    sampler(sampler&& other) noexcept;
    sampler(const sampler&) = delete;

    //--------------------------------------------------------------------------

    ~sampler();

    //--------------------------------------------------------------------------

    auto operator=(sampler&& other) noexcept -> sampler&;
    auto operator=(const sampler&) -> sampler& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the OpenGL name of this sampler
    ///
    /// \return the name of the sampler
    auto id() const noexcept -> ::GLuint;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    explicit sampler(::GLuint id) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_id;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_TEXTURE_SAMPLER_HPP */
