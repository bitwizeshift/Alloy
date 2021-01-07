////////////////////////////////////////////////////////////////////////////////
/// \file string_utilities.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_UTILITIES_STRING_UTILITIES_HPP
#define ALLOY_CORE_UTILITIES_STRING_UTILITIES_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/utilities/string_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/core/precision/real.hpp"

#include <cstdint>
#include <system_error> // std::error_code

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of miscellaneous string utilities
  /////////////////////////////////////////////////////////////////////////////
  struct ALLOY_CORE_API string_utilities
  {
    string_utilities() = delete;
    ~string_utilities() = delete;

    //-------------------------------------------------------------------------
    // Signed Integer Parsing
    //-------------------------------------------------------------------------

    /// \{
    /// \brief Converts the given string_view \p in to a signed integer
    ///        representation
    ///
    /// This only converts integers that are in decimal format.
    ///
    /// On failure, the return type will contain an error_code indicating the
    /// failure type.
    ///
    /// \param in the input value
    /// \return the integer on success
    static auto to_int8(string_view in) noexcept -> result<std::int8_t,std::error_code>;
    static auto to_int16(string_view in) noexcept -> result<std::int16_t,std::error_code>;
    static auto to_int32(string_view in) noexcept -> result<std::int32_t,std::error_code>;
    static auto to_int64(string_view in) noexcept -> result<std::int64_t,std::error_code>;
    /// \}

    //-------------------------------------------------------------------------
    // Unsigned Integer Parsing
    //-------------------------------------------------------------------------

    /// \{
    /// \brief Converts the given string_view \p in to an unsigned integer
    ///        representation
    ///
    /// This only converts integers that are in decimal format.
    ///
    /// On failure, the return type will contain an error_code indicating the
    /// failure type.
    ///
    /// \param in the input value
    /// \return the integer on success
    static auto to_uint8(string_view in) noexcept -> result<std::uint8_t,std::error_code>;
    static auto to_uint16(string_view in) noexcept -> result<std::uint16_t,std::error_code>;
    static auto to_uint32(string_view in) noexcept -> result<std::uint32_t,std::error_code>;
    static auto to_uint64(string_view in) noexcept -> result<std::uint64_t,std::error_code>;
    /// \}

    //-------------------------------------------------------------------------
    // Floating Point Parsing
    //-------------------------------------------------------------------------

    /// \{
    /// \brief Converts the given string_view \p in to a floating point
    ///        representation
    ///
    /// \param in the input value
    /// \return the integer on success
    static auto to_float(string_view in) noexcept -> result<float,std::error_code>;
    static auto to_double(string_view in) noexcept -> result<double,std::error_code>;
    static auto to_real(string_view in) noexcept -> result<real,std::error_code>;
    /// \}
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_UTILITIES_STRING_UTILITIES_HPP */
