// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/error.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/zstring_view.hpp"
#include <memory>
#include <string>
#include <system_error>
#include <utility>

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

    auto name() const noexcept -> const char* override
    {
      return "gl extended";
    }

    auto message(int error) const -> std::string override
    {
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

auto alloy::render::gl::error::gl_basic_category() -> const std::error_category&
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

alloy::render::gl::gl_error_message::gl_error_message(
  std::shared_ptr<core::string> message
) noexcept
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
