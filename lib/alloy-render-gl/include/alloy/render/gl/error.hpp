// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file error.hpp
///
/// \brief This header defines the errors that OpenGL itself reports
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_ERROR_HPP
#define ALLOY_RENDER_GL_ERROR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <GL/glew.h>

#include <system_error> // std::error_category, std::error_code
#include <type_traits>  // std::true_type

namespace alloy::render::gl {

  //===========================================================================
  // non-member functions : errors
  //===========================================================================

  namespace error {

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The errors that `glGetError` may report
    ///////////////////////////////////////////////////////////////////////////
    enum basic_errors : ::GLenum {
      invalid_enum = GL_INVALID_ENUM,
      invalid_value = GL_INVALID_VALUE,
      invalid_operation = GL_INVALID_OPERATION,
      stack_overflow = GL_STACK_OVERFLOW,
      stack_underflow = GL_STACK_UNDERFLOW,
      out_of_memory = GL_OUT_OF_MEMORY,
      invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
      context_lost = GL_CONTEXT_LOST,
      table_too_large = GL_TABLE_TOO_LARGE,
    };

    auto gl_basic_category() -> const std::error_category&;

    auto make_error_code(basic_errors error) noexcept -> std::error_code;

  } // namespace error

} // namespace alloy::render::gl

template <>
struct std::is_error_code_enum<alloy::render::gl::error::basic_errors>
  : std::true_type{};

#endif /* ALLOY_RENDER_GL_ERROR_HPP */
