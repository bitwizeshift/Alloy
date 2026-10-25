// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file presentation_target.hpp
///
/// \brief This header defines the interface of anything that presents the
///        default framebuffer of an OpenGL context, such as a window
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SURFACE_PRESENTATION_TARGET_HPP
#define ALLOY_RENDER_GL_SURFACE_PRESENTATION_TARGET_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstdint> // std::uint32_t

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The size of the area that is drawn into, in pixels
  //////////////////////////////////////////////////////////////////////////////
  struct surface_extent
  {
    /// The number of pixels across
    std::uint32_t width;

    /// The number of pixels down
    std::uint32_t height;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An interface to anything that presents the default framebuffer
  ///        of the OpenGL context that a device draws with
  ///
  /// This is what allows a window, or anything like one, to be drawn into
  /// without OpenGL knowing anything about the system that provides it.
  //////////////////////////////////////////////////////////////////////////////
  class presentation_target
  {
    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    virtual ~presentation_target() = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the size of the default framebuffer, in pixels
    ///
    /// This may differ from the size of a window in screen coordinates on
    /// displays with a high pixel density.
    ///
    /// \return the extent of the default framebuffer
    virtual auto drawable_extent() const -> surface_extent = 0;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Presents everything drawn into the default framebuffer since
    ///        it was last presented
    virtual auto present() -> void = 0;

    //--------------------------------------------------------------------------
    // Protected Constructors / Assignment
    //--------------------------------------------------------------------------
  protected:

    presentation_target() noexcept = default;
    presentation_target(presentation_target&&) noexcept = default;
    presentation_target(const presentation_target&) noexcept = default;

    //--------------------------------------------------------------------------

    auto operator=(presentation_target&&)
      noexcept -> presentation_target& = default;
    auto operator=(const presentation_target&)
      noexcept -> presentation_target& = default;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SURFACE_PRESENTATION_TARGET_HPP */
