// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/error.hpp"

#include "alloy/core/intrinsics.hpp"

#include <string> // std::string

// Disable this warning for the error_category message function.
// It is legal for enums to take on values outside of the switch case, and
// in this case the input can be any numeric value (due to the way that
// std::error_category works)
ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(-Wcovered-switch-default)

namespace {

  //===========================================================================
  // class : error_category
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Error category for the io_error enum
  /////////////////////////////////////////////////////////////////////////////
  class error_category final
    : public std::error_category
  {
  public:
    auto name() const noexcept -> const char* override
    {
      return "io error";
    }

    auto message(int ec) const -> std::string override
    {
      using alloy::core::io_error;

      const auto code = static_cast<io_error>(ec);

      switch (code) {
        case io_error::unexpected_eof: {
          return "end of stream reached before all bytes were read";
        }
        case io_error::write_zero: {
          return "writer accepted no bytes before all bytes were written";
        }
        default: {
          break;
        }
      }
      return "unknown error code";
    }
  };

} // namespace <anonymous>

ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()

//=============================================================================
// non-member functions : enum : io_error
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::core::io_error_category() noexcept -> const std::error_category&
{
  static const auto s_category = ::error_category{};

  return s_category;
}

auto alloy::core::make_error_code(io_error e) noexcept -> std::error_code
{
  return std::error_code{static_cast<int>(e), io_error_category()};
}
