// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file cull_face.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MATERIAL_CULL_FACE_HPP
#define ALLOY_RENDER_GL_MATERIAL_CULL_FACE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/material/cull_face.hpp"

#include "alloy/core/intrinsics.hpp"

#include <optional>
#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy cull_face \p b to an OpenGL
  ///        enum
  ///
  /// \param c the cull face
  /// \return the GLenum for the primitive
  constexpr
  auto to_opengl_type(cull_face c) noexcept -> std::optional<::GLenum>;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_type(cull_face c)
  noexcept -> std::optional<::GLenum>
{
  switch (c) {
    case cull_face::none: {
      return std::nullopt;
    }
    case cull_face::front: {
      return GL_FRONT;
    }
    case cull_face::back: {
      return GL_BACK;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MATERIAL_CULL_FACE_HPP */
