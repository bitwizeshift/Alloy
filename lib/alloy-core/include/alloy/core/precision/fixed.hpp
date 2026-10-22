// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file fixed.hpp
 *
 * \brief This header contains the definition of a fixed-precision type
 *****************************************************************************/

#ifndef ALLOY_CORE_PRECISION_FIXED_HPP
#define ALLOY_CORE_PRECISION_FIXED_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cassert>
#include <cstddef> // std::size_t

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
  /// \return `true` if \p f is a `nan`
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  auto is_nan(const basic_fixed<MantissaBits,ExponentBits>& f) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given basic_fixed value \p f is finite
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `finite`
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  auto is_finite(const basic_fixed<MantissaBits,ExponentBits>& f)
    noexcept -> bool;

  /// \brief Determines whether a given basic_fixed value \p f is infinite
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `infinite`
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  auto is_infinite(const basic_fixed<MantissaBits,ExponentBits>& f)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given basic_fixed value \p f is normal
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `normal`
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  auto is_normal(const basic_fixed<MantissaBits,ExponentBits>& f)
    noexcept -> bool;

  /// \brief Determines whether a given basic_fixed value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `subnormal`
  template<std::size_t MantissaBits, std::size_t ExponentBits>
  auto is_subnormal(const basic_fixed<MantissaBits,ExponentBits>& f)
    noexcept -> bool;

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
inline
auto alloy::core::is_nan(const basic_fixed<MantissaBits,ExponentBits>& f)
  noexcept -> bool
{
  (void) f;
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline
auto alloy::core::is_finite(const basic_fixed<MantissaBits,ExponentBits>& f)
  noexcept -> bool
{
  (void) f;
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline
auto alloy::core::is_infinite(const basic_fixed<MantissaBits,ExponentBits>& f)
  noexcept -> bool
{
  (void) f;
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline
auto alloy::core::is_normal(const basic_fixed<MantissaBits,ExponentBits>& f)
  noexcept -> bool
{
  (void) f;
  assert( false && "not implemented" );
  return false;
}

template<std::size_t MantissaBits, std::size_t ExponentBits>
inline
auto alloy::core::is_subnormal(const basic_fixed<MantissaBits,ExponentBits>& f)
  noexcept -> bool
{
  (void) f;
  assert( false && "not implemented" );
  return false;
}

//==============================================================================
// definitions : literals
//==============================================================================

// TODO(bitwizeshift) determine what reasonable literals can be provided
//                    for fixed-precision type. Probably match the aliases?

#endif /* ALLOY_CORE_PRECISION_FIXED_HPP */

