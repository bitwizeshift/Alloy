// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file fnv1a_hash.hpp
 *
 * \brief This header defines a utility for managing function dispatch
 *****************************************************************************/

#ifndef ALLOY_CORE_UTILITIES_FNV1A_HASH_HPP
#define ALLOY_CORE_UTILITIES_FNV1A_HASH_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstdint> // std::uint32_t, std::uint64_t
#include <cstddef> // std::byte, std::size_t
#include <type_traits> // sizeof checks

namespace alloy::core {

  //===========================================================================
  // trait : fnv1a_traits
  //===========================================================================

  /// \brief Triats to determine information about the fnv1a hash
  ///
  /// \tparam Bits the number of bits of the underlying integer to
  ///         hash
  template <std::size_t Bits>
  struct fnv1a_traits;

  template <>
  struct fnv1a_traits<32u>
  {
    using hash_type = std::uint32_t;
    inline static constexpr hash_type offset = 2166136261;
    inline static constexpr hash_type prime = 16777619;
  };

  template <>
  struct fnv1a_traits<64u>
  {
    using hash_type = std::uint64_t;
    inline static constexpr hash_type offset = 14695981039346656037ull;
    inline static constexpr hash_type prime = 1099511628211ull;
  };

  //===========================================================================
  // non-member functions : trait : fnv1a_hash
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Hashes the given sequence of size \p bytes using the
  ///        Fowler-Noll-Vo hash
  ///
  /// This function may be used to compute either the 32 or 64 bit
  /// hash values.
  ///
  /// \tparam Bits the number of bits to use in this fnv1a hash
  /// \param p a pointer to data
  /// \param bytes the number of bytes to process
  template <std::size_t Bits, typename Byte>
    requires (sizeof(Byte) == 1)
  constexpr auto fnv1a_hash(const Byte* p, std::size_t bytes)
    noexcept -> typename fnv1a_traits<Bits>::hash_type;

  /// \brief Hashes a null-terminated sequence of bytes using the
  ///        Fowler-Noll-Vo hash
  ///
  /// This function may be used to compute either the 32 or 64 bit
  /// hash values.
  ///
  /// \tparam Bits the number of bits to use in this fnv1a hash
  /// \param p a pointer to null-terminated data
  template <std::size_t Bits, typename Byte>
    requires (sizeof(Byte) == 1)
  constexpr auto fnv1a_hash(const Byte* p)
    noexcept -> typename fnv1a_traits<Bits>::hash_type;

} // namespace alloy::core

//=============================================================================
// definitions : non-member functions : trait : fnv1a_hash
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <std::size_t Bits, typename Byte>
  requires (sizeof(Byte) == 1)
inline constexpr
auto alloy::core::fnv1a_hash(const Byte* p, std::size_t bytes)
  noexcept -> typename fnv1a_traits<Bits>::hash_type
{
  using traits_type = alloy::core::fnv1a_traits<Bits>;

  auto result = traits_type::offset;
  while (bytes--) {
    result ^= *p++;
    result *= traits_type::prime;
  }
  return result;
}

template <std::size_t Bits, typename Byte>
  requires (sizeof(Byte) == 1)
inline constexpr
auto alloy::core::fnv1a_hash(const Byte* p)
  noexcept -> typename fnv1a_traits<Bits>::hash_type
{
  using traits_type = alloy::core::fnv1a_traits<Bits>;

  auto result = traits_type::offset;
  while (*p != Byte{0}) {
    result ^= *p++;
    result *= traits_type::prime;
  }
  return result;
}

#endif /* ALLOY_CORE_UTILITIES_FNV1A_HASH_HPP */
