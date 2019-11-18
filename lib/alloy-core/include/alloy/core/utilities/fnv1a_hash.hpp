/*****************************************************************************
 * \file fnv1a_hash.hpp
 *
 * \brief This header defines a utility for managing function dispatch
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
#ifndef ALLOY_CORE_UTILITIES_FNV1A_HASH_HPP
#define ALLOY_CORE_UTILITIES_FNV1A_HASH_HPP

#include <cstdint> // std::uint32_t, std::uint64_t
#include <cstddef> // std::byte, std::size_t
#include <type_traits> // std::enable_if

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
  template <std::size_t Bits, typename Byte,
            typename = std::enable_if_t<sizeof(Byte)==1>>
  constexpr typename fnv1a_traits<Bits>::hash_type
    fnv1a_hash(const Byte* p, std::size_t bytes) noexcept;

  /// \brief Hashes a null-terminated sequence of bytes using the
  ///        Fowler-Noll-Vo hash
  ///
  /// This function may be used to compute either the 32 or 64 bit
  /// hash values.
  ///
  /// \tparam Bits the number of bits to use in this fnv1a hash
  /// \param p a pointer to null-terminated data
  template <std::size_t Bits, typename Byte,
            typename = std::enable_if_t<sizeof(Byte)==1>>
  constexpr typename fnv1a_traits<Bits>::hash_type
    fnv1a_hash(const Byte* p) noexcept;

} // namespace alloy::core

//=============================================================================
// definitions : non-member functions : trait : fnv1a_hash
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <std::size_t Bits, typename Byte, typename>
inline constexpr typename alloy::core::fnv1a_traits<Bits>::hash_type
  alloy::core::fnv1a_hash(const Byte* p, std::size_t bytes)
  noexcept
{
  using traits_type = alloy::core::fnv1a_traits<Bits>;

  auto result = traits_type::offset;
  while (bytes--) {
    result ^= *p++;
    result *= traits_type::prime;
  }
  return result;
}

template <std::size_t Bits, typename Byte, typename>
inline constexpr typename alloy::core::fnv1a_traits<Bits>::hash_type
  alloy::core::fnv1a_hash(const Byte* p)
  noexcept
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