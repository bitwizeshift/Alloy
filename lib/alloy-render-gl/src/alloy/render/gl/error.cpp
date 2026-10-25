// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/error.hpp"

#include <GL/glew.h>

#include <string>       // std::string, std::to_string
#include <system_error> // std::error_category, std::error_code

//=============================================================================
// non-member functions : errors
//=============================================================================

namespace alloy::render::gl::error {

  //===========================================================================
  // class : gl_basic_error_category
  //===========================================================================

  class gl_basic_error_category : public std::error_category
  {
  public:

    auto name() const noexcept -> const char* override
    {
      return "gl";
    }

    auto message(int error) const -> std::string override
    {
      switch (static_cast<::GLenum>(error)) {
        case invalid_enum: {
          return "invalid enum";
        }
        case invalid_value: {
          return "invalid value";
        }
        case invalid_operation: {
          return "invalid operation";
        }
        case stack_overflow: {
          return "stack overflow";
        }
        case stack_underflow: {
          return "stack underflow";
        }
        case out_of_memory: {
          return "out of memory";
        }
        case invalid_framebuffer_operation: {
          return "invalid framebuffer operation";
        }
        case context_lost: {
          return "context lost";
        }
        case table_too_large: {
          return "table too large";
        }
        default: {
          break;
        }
      }
      return "unknown OpenGL error " + std::to_string(error);
    }
  };

} // namespace alloy::render::gl::error

auto alloy::render::gl::error::gl_basic_category() -> const std::error_category&
{
  static const auto s_category = gl_basic_error_category{};

  return s_category;
}

auto alloy::render::gl::error::make_error_code(basic_errors error)
  noexcept -> std::error_code
{
  return std::error_code{static_cast<int>(error), gl_basic_category()};
}
