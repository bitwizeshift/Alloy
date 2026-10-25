// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file texture.hpp
///
/// \brief This header defines an OpenGL texture object
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_TEXTURE_TEXTURE_HPP
#define ALLOY_RENDER_GL_TEXTURE_TEXTURE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/capabilities.hpp"

#include "alloy/core/utilities/result.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/texture/texture_dimension.hpp"
#include "alloy/render/texture/texture_format.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of an OpenGL texture object
  //////////////////////////////////////////////////////////////////////////////
  class texture
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes the texture described by \p spec
    ///
    /// The texture is left bound to its target on the active texture unit.
    ///
    /// \param spec the texture spec
    /// \param caps the capabilities of the current context
    /// \return the texture on success, or
    ///         `render_errc::unsupported_texture_format` if the context
    ///         cannot create it
    static auto make(const texture_spec& spec, const capabilities& caps)
      -> core::result<texture, render_error>;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    texture(texture&& other) noexcept;
    texture(const texture&) = delete;

    //--------------------------------------------------------------------------

    ~texture();

    //--------------------------------------------------------------------------

    auto operator=(texture&& other) noexcept -> texture&;
    auto operator=(const texture&) -> texture& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the OpenGL name of this texture
    ///
    /// \return the name of the texture
    auto id() const noexcept -> ::GLuint;

    /// \brief Gets the OpenGL target that this texture binds to
    ///
    /// \return the target of the texture
    auto target() const noexcept -> ::GLenum;

    /// \brief Gets the dimension of this texture
    ///
    /// \return the dimension of the texture
    auto dimension() const noexcept -> texture_dimension;

    /// \brief Gets the format of this texture
    ///
    /// \return the format of the texture
    auto format() const noexcept -> texture_format;

    /// \brief Gets the extent of the base level of this texture
    ///
    /// \return the extent of the texture
    auto extent() const noexcept -> texture_extent;

    /// \brief Gets the number of samples per texel of this texture
    ///
    /// \return the sample count of the texture
    auto samples() const noexcept -> sample_count;

    /// \brief Gets the ways in which this texture may be used
    ///
    /// \return the usages of the texture
    auto usage() const noexcept -> texture_usages;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    texture(::GLuint id, ::GLenum target, const texture_spec& spec) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_id;
    ::GLenum m_target;
    texture_dimension m_dimension;
    texture_format m_format;
    texture_extent m_extent;
    sample_count m_samples;
    texture_usages m_usage;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_TEXTURE_TEXTURE_HPP */
