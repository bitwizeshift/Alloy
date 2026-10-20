// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file half.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_PRECISION_HALF_HPP
#define ALLOY_CORE_PRECISION_HALF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cassert>

namespace alloy::core {

  // TODO(bitwize): Implement 'half' precision type
  class half;

  //============================================================================
  // non-member functions : class : half
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Determines whether a given half value \p f is a nan
  ///
  /// \param f the value to check
  /// \return `true` if \p f is a `nan`
  bool is_nan( const half& f ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given half value \p f is finite
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `finite`
  bool is_finite( const half& f ) noexcept;

  /// \brief Determines whether a given half value \p f is infinite
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `infinite`
  bool is_infinite( const half& f ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given half value \p f is normal
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `normal`
  bool is_normal( const half& f ) noexcept;

  /// \brief Determines whether a given half value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `subnormal`
  bool is_subnormal( const half& f ) noexcept;

  //============================================================================
  // literals
  //============================================================================

  inline namespace literals {
    inline namespace precision_literals {

      // cannot be implemented without definition of 'half'

      // half operator""_h(long double in) noexcept;
      // half operator""_half(long double in) noexcept;

    } // inline namespace precision_literals
  } // inline namespaec literals

} // namespace alloy::core

//==============================================================================
// definitions : non-member functions : alias : half
//==============================================================================

inline bool alloy::core::is_nan( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  (void) f;
  return false;
}

inline bool alloy::core::is_finite( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  (void) f;
  return false;
}

inline bool alloy::core::is_infinite( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  (void) f;
  return false;
}

inline bool alloy::core::is_normal( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  (void) f;
  return false;
}

inline bool alloy::core::is_subnormal( const half& f )
  noexcept
{
  assert( false && "not implemented" );
  (void) f;
  return false;
}

//==============================================================================
// definitions : literals
//==============================================================================

// inline constexpr alloy::core::half
//   alloy::core::literals::precision_literals::operator""_h(long double in)
//   noexcept
// {
//   return half{in};
// }
//
// inline constexpr alloy::core::half
//   alloy::core::literals::precision_literals::operator""_half(long double in)
//   noexcept
// {
//   return half{in};
// }

#endif /* ALLOY_CORE_PRECISION_HALF_HPP */

