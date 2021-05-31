/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "alloy/render/gl/error.hpp"

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

    auto name() const noexcept -> const char* override {
      return "gl";
    }

    auto message(int error) const -> std::string override {
      const auto* s = ::glGetString(static_cast<::GLenum>(error));

      return std::string{reinterpret_cast<const char*>(s)};
    }
  };

  //===========================================================================
  // class : gl_extended_error_category
  //===========================================================================

  class gl_extended_error_category : public std::error_category
  {
  public:

    auto name() const noexcept -> const char* override {
      return "gl extended";
    }

    auto message(int error) const -> std::string override {
      switch (error) {
        case 0: {
          return "none";
        }
        case uniform_not_found: {
          return "uniform not found";
        }
        case attribute_not_found: {
          return "attribute not found";
        }
        default: break;
      }
      return "error code " + std::to_string(error) + " not found";
    }
  };

} // namespace alloy::render::gl::error

auto alloy::render::gl::error::gl_basic_category()
  -> const std::error_category&
{
  static const auto s_category = gl_basic_error_category{};

  return s_category;
}

auto alloy::render::gl::error::gl_extended_category()
  -> const std::error_category&
{
  static const auto s_category = gl_extended_error_category{};

  return s_category;
}

auto alloy::render::gl::error::make_error_code(basic_errors error)
  noexcept -> std::error_code
{
  return std::error_code{static_cast<int>(error), gl_basic_category()};
}

auto alloy::render::gl::error::make_error_code(extended_errors error)
  noexcept -> std::error_code
{
  return std::error_code{static_cast<int>(error), gl_extended_category()};
}

//=============================================================================
// class : gl_error_message
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

auto alloy::render::gl::gl_error_message::make(core::string message)
  -> gl_error_message
{
  // There appears to be a bug in std::allocate_shared that prevents
  // calling allocate_shared<core::string> due to the use of
  // "explicitly_copyable", where it is erroneously thinking that the T type
  // is _Sp_counted_ptr_inplace.
  auto ptr = std::make_shared<core::string>(
    std::move(message)
  );

  return gl_error_message{std::move(ptr)};
}

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

alloy::render::gl::gl_error_message::gl_error_message(std::shared_ptr<core::string> message)
  noexcept
  : m_message{std::move(message)}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::render::gl::gl_error_message::message()
  const noexcept -> core::zstring_view
{
  if (m_message != nullptr) {
    return *m_message;
  }
  return "no error";
}
