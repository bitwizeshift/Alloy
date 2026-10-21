// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file error.hpp
///
/// \brief This header contains the errors that are produced by the reading
///        and writing utilities
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_ERROR_HPP
#define ALLOY_CORE_IO_ERROR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"

#include <system_error> // std::error_code, std::error_category
#include <type_traits>  // std::true_type

namespace alloy::core {

  //===========================================================================
  // enum : io_error
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Errors that are produced by the io utilities themselves, rather
  ///        than by an underlying reader or writer.
  ///
  /// Readers and writers may fail with any std::error_code; these errors only
  /// describe the conditions that the io utilities detect while composing
  /// reads and writes.
  /////////////////////////////////////////////////////////////////////////////
  enum class io_error
  {
    unexpected_eof = 1, ///< The end of the stream was reached before the
                        ///< requested number of bytes could be read
    write_zero     = 2, ///< A writer accepted no bytes before all of the
                        ///< requested bytes could be written
  };

  //===========================================================================
  // non-member functions : enum : io_error
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Gets the error category of io errors
  ///
  /// \return a reference to the io error category
  ALLOY_CORE_API
  auto io_error_category() noexcept -> const std::error_category&;

  /// \brief Makes an error code from the given io_error \p e
  ///
  /// \param e the io error
  /// \return the std::error_code of the io error
  ALLOY_CORE_API
  auto make_error_code(io_error e) noexcept -> std::error_code;

} // namespace alloy::core

namespace std {
  template <>
  struct is_error_code_enum<alloy::core::io_error> : std::true_type{};
} // namespace std

#endif /* ALLOY_CORE_IO_ERROR_HPP */
