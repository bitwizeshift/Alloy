/*****************************************************************************
 * \file fixed.hpp
 *
 * \brief This header contains the definition of a fixed-precision type
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
#ifndef ALLOY_CORE_PRECISION_FIXED_HPP
#define ALLOY_CORE_PRECISION_FIXED_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef> // std::size_t
#include <cassert>

namespace alloy::core {

  template<std::size_t MantissaBits, std::size_t ExponentBits>
  class basic_fixed;

  //============================================================================
  // non-member functions : class : basic_fixed
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Determines whether a given basic_fixed value \p f is a nan
  ///
  /// \param f the value to check
  /// \return \c true if \p f is a \c nan
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  bool is_nan( const basic_fixed<MantissaBits,ExponentBits>& f ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given basic_fixed value \p f is finite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c finite
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  bool is_finite( const basic_fixed<MantissaBits,ExponentBits>& f ) noexcept;

  /// \brief Determines whether a given basic_fixed value \p f is infinite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c infinite
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  bool is_infinite( const basic_fixed<MantissaBits,ExponentBits>& f ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given basic_fixed value \p f is normal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c normal
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  bool is_normal( const basic_fixed<MantissaBits,ExponentBits>& f ) noexcept;

  /// \brief Determines whether a given basic_fixed value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c subnormal
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  bool is_subnormal( const basic_fixed<MantissaBits,ExponentBits>& f ) noexcept;

  //============================================================================
  // aliases
  //============================================================================

  // TODO(bitwizeshift) determine what reasonable defaults can be made for
  //                    fixed-precision type

  //============================================================================
  // literals
  //============================================================================

  inline namespace literals {
    inline namespace precision_literals {

      // TODO(bitwizeshift) determine what reasonable literals can be provided
      //                    for fixed-precision type. Probably match the
      //                    aliases?

    } // inline namespace precision_literals
  } // inline namespaec literals

} // namespace alloy::core

//==============================================================================
// definitions : non-member functions : alias : basic_fixed
//==============================================================================

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_nan( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_finite( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_infinite( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_normal( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline bool
  alloy::core::is_subnormal( const basic_fixed<MantissaBits,ExponentBits>& f )
  noexcept
{
  assert( false && "not implemented" );
  return false;
}

//==============================================================================
// definitions : literals
//==============================================================================

// TODO(bitwizeshift) determine what reasonable literals can be provided
//                    for fixed-precision type. Probably match the aliases?

#endif /* ALLOY_CORE_PRECISION_FIXED_HPP */