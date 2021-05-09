/*****************************************************************************
 * \file source_location.hpp
 *
 * \brief This header defines a utility for handling the location of a source
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_UTILITIES_SOURCE_LOCATION_HPP
#define ALLOY_CORE_UTILITIES_SOURCE_LOCATION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstdint>     // std::uint_least32_t

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Represents the location of errors in source code
  /////////////////////////////////////////////////////////////////////////////
  class source_location
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    constexpr source_location() noexcept = default;
    constexpr source_location(const char* filename,
                              const char* function,
                              std::uint_least32_t line_number) noexcept;
    constexpr source_location(const source_location&) noexcept = default;
    constexpr source_location& operator=(const source_location&) noexcept = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the file that this source location represents
    ///
    /// \return the file
    constexpr auto file_name() const noexcept -> const char*;

    /// \brief Gets the function that this source location represents
    ///
    /// \return the function
    constexpr auto function_name() const noexcept -> const char*;

    /// \brief Gets the line that this source location represents
    ///
    /// \return the line
    constexpr auto line() const noexcept -> std::uint_least32_t;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    const char* m_filename{};
    const char* m_function{};
    std::uint_least32_t m_line_number{};
  };


//! \def ALLOY_CURRENT_SOURCE_LOCATION()
//!
//! \brief Creates an alloy::core::source_location that refers to the line in
//!        which this macro is invoked
#define ALLOY_CURRENT_SOURCE_LOCATION() \
  ::alloy::core::source_location { __FILE__, __func__, __LINE__ }

} // namespace alloy::core

//=============================================================================
// class : source_location
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline constexpr
alloy::core::source_location::source_location(const char* filename,
                                              const char* function,
                                              std::uint_least32_t line_number)
  noexcept
  : m_filename{filename},
    m_function{function},
    m_line_number{line_number}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::source_location::file_name()
  const noexcept -> const char*
{
  return m_filename;
}

inline constexpr
auto alloy::core::source_location::function_name()
  const noexcept -> const char*
{
  return m_function;
}

inline constexpr
auto alloy::core::source_location::line()
  const noexcept -> std::uint_least32_t
{
  return m_line_number;
}

#endif /* ALLOY_CORE_UTILITIES_SOURCE_LOCATION_HPP */