/*****************************************************************************
 * \file int_utilities.hpp
 *
 * \brief This header defines utilities relating to construction, composition,
 *        and decomposition of integral values.
 *****************************************************************************/

/*
 The MIT License (MIT)
 Copyright (c) 2021 Matthew Rodusek. All rights reserved.
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the \Software\), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED \AS IS\ WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/
#ifndef ALLOY_CORE_UTILITIES_INT_UTILITIES_HPP
#define ALLOY_CORE_UTILITIES_INT_UTILITIES_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"
#include "alloy/core/intrinsics.hpp"

#include <type_traits> // std::make_signed
#include <array>       // std::array

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of useful utilities for integral values
  //////////////////////////////////////////////////////////////////////////////
  class int_utilities final
  {
    int_utilities() = delete;
    ~int_utilities() = delete;

    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes an `Int` by concatenating the input \p ints
    ///
    /// The sum of all the ints must be equal to `sizeof(Int)`
    ///
    /// \tparam Int the type to construct. Must be either integral or enum.
    /// \param ints the integers to concatenate. Size of each type must sum to
    ///             the size of the receiver
    /// \return the integer
    template <typename Int, typename...Ints,
              typename = std::enable_if_t<sizeof(Int) == (sizeof(Ints) + ...)>>
    static constexpr auto make(const Ints&...ints) noexcept -> Int;

    /// \brief Converts an `s8` into a `u8`
    ///
    /// \param v the `s8` to convert
    /// \return the `u8` representation of \p v
    static constexpr auto make_u8(s8 v) noexcept -> u8;

    /// \brief Identity function
    ///
    /// \param b the byte
    /// \return \p b
    static constexpr auto make_u8(u8 b) noexcept -> u8;

    //--------------------------------------------------------------------------

    /// \brief Converts an `s16`'s representation into a `u16`
    ///
    /// \param v the `s16` to convert
    /// \return the `u16` representation of \p v
    static constexpr auto make_u16(s16 v) noexcept -> u16;

    /// \brief Identity function
    ///
    /// \param v the `u16`
    /// \return \p v
    static constexpr auto make_u16(u16 v) noexcept -> u16;

    /// \brief Creates a `u16` by concatenating two `u8`s together
    ///
    /// The result is a `u16` formed from `[b0,b1]`.
    ///
    /// \note
    /// Concatenation is done from left-to-right, with the first value starting
    /// at the most-significant byte. Thus an example input of:
    /// `0x12,0x56` produces `0x1234`
    ///
    /// \param b0 the high byte of the `u16`
    /// \param b1 the low byte of the `u16`
    /// \return the concatenated `u16`
    static constexpr auto make_u16(u8 b0, u8 b1) noexcept -> u16;

    //--------------------------------------------------------------------------

    /// \brief Converts an `s32`'s representation into a `u32`
    ///
    /// \param v the `s32` to convert
    /// \return the `u32` representation of \p v
    static constexpr auto make_u32(s32 v) noexcept -> u32;

    /// \brief Identity function
    ///
    /// \param v the `u32`
    /// \return \p v
    static constexpr auto make_u32(u32 v) noexcept -> u32;

    /// \brief Creates a `u32` by concatenating two `u16`s together
    ///
    /// The result is a `u32` formed from `[v0,v1]`
    ///
    /// \note
    /// Concatenation is done from left-to-right, with the first value starting
    /// at the most-significant byte. Thus an example input of:
    /// `0x1234,0x5678` produces `0x12345678`
    ///
    /// \param v0 the high `u16` of the `u32`
    /// \param v1 the low `u16` of the `u32`
    /// \return the concatenated `u32`
    static constexpr auto make_u32(u16 v0, u16 v1) noexcept -> u32;

    /// \brief Creates a `u32` by concatenating four `uu`s together
    ///
    /// The result is a `u32` formed from `[b0,b1,b2,b3]`
    ///
    /// \note
    /// Concatenation is done from left-to-right, with the first value starting
    /// at the most-significant byte. Thus an example input of:
    /// `0x12,0x34,0x56,0x78` produces `0x12345678`
    ///
    /// \param b0 the high `u8` of the `u32`
    /// \param b1 the second high `u8` of the `u32`
    /// \param b2 the second low `u8` of the `u32`
    /// \param b3 the low `u8` of the `u32`
    /// \return the concatenated `u32`
    static constexpr auto make_u32(u8 b0, u8 b1, u8 b2, u8 b3) noexcept -> u32;

    //--------------------------------------------------------------------------

    /// \brief Converts an `s64`'s representation into a `u64`
    ///
    /// \param v the `s64` to convert
    /// \return the `u64` representation of \p v
    static constexpr auto make_u64(s64 v) noexcept -> u64;

    /// \brief Identity function
    ///
    /// \param v the `u64`
    /// \return \p v
    static constexpr auto make_u64(u64 v) noexcept -> u64;

    /// \brief Creates a `u64` by concatenating two `u32`s together
    ///
    /// The result is a `u64` formed from `[v0,v1]`
    ///
    /// \note
    /// Concatenation is done from left-to-right, with the first value starting
    /// at the most-significant byte. Thus an example input of:
    /// `0x12345678,0x9abcdef0` produces `0x123456789abcdef0`
    ///
    /// \param v0 the high `u32` of the `u64`
    /// \param v1 the low `u32` of the `u64`
    /// \return the concatenated `u64`
    static constexpr auto make_u64(u32 v0, u32 v1) noexcept -> u64;

    /// \brief Creates a `u64` by concatenating four `u16`s together
    ///
    /// The result is a `u64` formed from `[v0,v1,v2,v3]`
    ///
    /// \note
    /// Concatenation is done from left-to-right, with the first value starting
    /// at the most-significant byte. Thus an example input of:
    /// `0x1234,0x5678,0x9abc,0xdef0` produces `0x123456789abcdef0`
    ///
    /// \param v0 the high `u16` of the `u64`
    /// \param v1 the second highest `u16` of the `u64`
    /// \param v2 the second lowest `u16` of the `u64`
    /// \param v3 the lowest `u16` of the `u64`
    /// \return the concatenated `u64`
    static constexpr auto make_u64(u16 v0, u16 v1, u16 v2, u16 v3) noexcept -> u64;

    /// \brief Creates a `u64` by concatenating eight `u8`s together
    ///
    /// The result is a `u64` formed from `[b0,b2,b3,b4,b5,b6,b7]`
    ///
    /// \note
    /// Concatenation is done from left-to-right, with the first value starting
    /// at the most-significant byte. Thus an example input of:
    /// `0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0` produces `0x123456789abcdef0`
    ///
    /// \param b0 the highest `u0`
    /// \param b1 the second `u0`
    /// \param b2 the third `u0`
    /// \param b3 the fourth `u0`
    /// \param b4 the fifth `u0`
    /// \param b5 the sixth `u0`
    /// \param b6 the seventh `u0`
    /// \param b7 the final `u0`
    /// \return the concatenated `u64`
    static constexpr auto make_u64(u8 b0, u8 b1, u8 b2, u8 b3, u8 b4, u8 b5, u8 b6, u8 b7) noexcept -> u64;

    //--------------------------------------------------------------------------
    // Splitting
    //--------------------------------------------------------------------------
  public:

    /// \brief Splits a given integral value \p from into a sequence of
    ///        smaller integral values \p To
    ///
    /// This is an easy way to convert an integral T value into an array of
    /// bytes.
    ///
    /// \note
    /// Splitting is meant as a "logical" operation which is irrespective of
    /// the underlying platform-endianness;  though it can be conceptually
    /// thought of as "big-endian" since the order of the bytes are preserved.
    /// Thus `0x12345678` can be split into either `[0x1234,0x5678]` or
    /// `[0x12,0x34,0x56,0x78]`.
    ///
    /// \tparam To The type to convert to. Must be either integral or an enum,
    ///         and 'From' must be divisible by it. Default: `u8`
    /// \param from the value to split
    /// \return the result of the split as an array
    template <typename To = u8, typename..., typename From>
    static constexpr auto split(const From& from)
      noexcept -> std::array<To,sizeof(From) / sizeof(To)>;

    /// \brief Splits a given integral value \p from into the output iterator
    ///        receiver of \p o
    ///
    /// \tparam To The type to convert to. Must be either integral or an enum,
    ///         and 'From' must be divisible by it. Default: `u8`
    /// \param o the iterator to split into
    /// \param from the value to split
    template <typename To = u8, typename OutputIt, typename From>
    static constexpr auto split_into(OutputIt o, const From& from) noexcept -> void;

    //--------------------------------------------------------------------------
    // Extraction
    //--------------------------------------------------------------------------
  public:

    /// \brief Extract the N'th `To` from the specified \p from
    ///
    /// Indices are from left to right, so a `From` can be converted to a
    /// `[To0,To1]`, `[To0,To1,To2,To3]`, etc.
    ///
    /// \tparam N the nth value
    /// \tparam To the type to convert to
    /// \param from the value to convert
    /// \return the To value
    template <std::size_t N, typename To = u8, typename From>
    static constexpr auto extract(const From& from) noexcept -> To;

    //--------------------------------------------------------------------------
    // Comparisons
    //--------------------------------------------------------------------------
  public:

    struct equal_type {
      using is_transparent = std::true_type;

      /// \brief Safely compares equality between two integer values
      ///
      /// This is guaranteed to provide the correct result irrespective of the
      /// sign of both integer values. This allows for a safe way to compare
      /// signed and unsigned integers.
      ///
      /// \param lhs the left integer
      /// \param rhs the right integer
      /// \return the result of the equality
      template <typename T, typename U>
      constexpr auto operator()(T lhs, U rhs) const noexcept -> bool;
    };
    struct not_equal_type {
      using is_transparent = std::true_type;

      /// \brief Safely compares inequality between two integer values
      ///
      /// This is guaranteed to provide the correct result irrespective of the
      /// sign of both integer values. This allows for a safe way to compare
      /// signed and unsigned integers.
      ///
      /// \param lhs the left integer
      /// \param rhs the right integer
      /// \return the result of the inequality
      template <typename T, typename U>
      constexpr auto operator()(T lhs, U rhs) const noexcept -> bool;
    };

    struct greater_type {
      using is_transparent = std::true_type;

      /// \brief Safely checks if `lhs` is greater than `rhs`
      ///
      /// This is guaranteed to provide the correct result irrespective of the
      /// sign of both integer values.
      ///
      /// \param lhs the left integer
      /// \param rhs the right integer
      /// \return the result of `lhs < rhs`
      template <typename T, typename U>
      constexpr auto operator()(T lhs, U rhs) const noexcept -> bool;
    };

    struct greater_equal_type {
      using is_transparent = std::true_type;

      /// \brief Safely checks if `lhs` is greater than or equal to `rhs`
      ///
      /// This is guaranteed to provide the correct result irrespective of the
      /// sign of both integer values.
      ///
      /// \param lhs the left integer
      /// \param rhs the right integer
      /// \return the result of `lhs <= rhs`
      template <typename T, typename U>
      constexpr auto operator()(T lhs, U rhs) const noexcept -> bool;
    };

    struct less_type {
      using is_transparent = std::true_type;

      /// \brief Safely checks if `lhs` is less than `rhs`
      ///
      /// This is guaranteed to provide the correct result irrespective of the
      /// sign of both integer values.
      ///
      /// \param lhs the left integer
      /// \param rhs the right integer
      /// \return the result of `lhs > rhs`
      template <typename T, typename U>
      constexpr auto operator()(T lhs, U rhs) const noexcept -> bool;
    };

    struct less_equal_type {
      using is_transparent = std::true_type;

      /// \brief Safely checks if `lhs` is less than or equal to `rhs`
      ///
      /// This is guaranteed to provide the correct result irrespective of the
      /// sign of both integer values.
      ///
      /// \param lhs the left integer
      /// \param rhs the right integer
      /// \return the result of `lhs >= rhs`
      template <typename T, typename U>
      constexpr auto operator()(T lhs, U rhs) const noexcept -> bool;
    };

    static constexpr inline auto equal = equal_type{};
    static constexpr inline auto not_equal = not_equal_type{};
    static constexpr inline auto greater = greater_type{};
    static constexpr inline auto greater_equal = greater_equal_type{};
    static constexpr inline auto less = greater_type{};
    static constexpr inline auto less_equal = greater_equal_type{};

    //--------------------------------------------------------------------------
    // Private: Static Factories
    //--------------------------------------------------------------------------
  private:

    template <typename Int, typename...Ints, std::size_t...Idxs>
    static constexpr auto make_impl(std::index_sequence<Idxs...>, const Ints&...ints)
      noexcept -> Int;

    //--------------------------------------------------------------------------
    // Private: Splitting
    //--------------------------------------------------------------------------
  private:

    template <typename To, std::size_t...Idxs, typename From>
    static constexpr auto split_impl(const From& from, std::index_sequence<Idxs...>)
      noexcept -> std::array<To,sizeof(From) / sizeof(To)>;

    template <typename To, typename OutputIt, typename From, std::size_t...Idxs>
    static constexpr auto split_into_impl(
      OutputIt o,
      const From& from,
      std::index_sequence<Idxs...>
    ) noexcept -> void;
  };

} // namespace alloy::core

namespace alloy::core::detail {

  template <typename T, bool = std::is_enum_v<T>>
  struct unsigned_underlying_type : std::make_unsigned<T>{};

  template <typename T>
  struct unsigned_underlying_type<T,true>
    : std::make_unsigned<std::underlying_type_t<T>>
  {};

  template <typename T>
  using unsigned_underlying_type_t = typename unsigned_underlying_type<T>::type;

  template <std::size_t N, typename Type0, typename...Types>
  constexpr auto offset_sum_of_sizes() -> std::size_t
  {
    if constexpr (N == 0u) {
      return (sizeof(Types) + ... + 0u);
    } else {
      return offset_sum_of_sizes<(N-1u), Types...>();
    }
  }

} // namespace alloy::core::detail

//------------------------------------------------------------------------------
// Private: Static Factories
//------------------------------------------------------------------------------

// This is not defined in the same order as the declaration due to a bug in
// clang that cannot be disabled. Apparently `-Wundefined-inline` triggers when
// this is defined below `::make(...)` but not when above.
template <typename Int, typename...Ints, std::size_t...Idxs>
inline constexpr
auto alloy::core::int_utilities::make_impl(
  std::index_sequence<Idxs...>,
  const Ints&...ints
) noexcept -> Int
{
  using to_type = typename detail::unsigned_underlying_type<Int>::type;

  constexpr auto offsets = std::array<std::size_t,sizeof...(Ints)>{{
    detail::offset_sum_of_sizes<Idxs,Ints...>() * 8u ...
  }};

  return static_cast<Int>(((static_cast<to_type>(ints) << offsets[Idxs]) | ...));
}

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

template <typename Int, typename... Ints, typename>
inline constexpr
auto alloy::core::int_utilities::make(const Ints&...ints)
  noexcept -> Int
{
  return make_impl<Int>(std::make_index_sequence<sizeof...(Ints)>{}, ints...);
}

inline constexpr
auto alloy::core::int_utilities::make_u8(s8 v)
  noexcept -> u8
{
  return make<s8>(v);
}

inline constexpr
auto alloy::core::int_utilities::make_u8(u8 b)
  noexcept -> u8
{
  return b;
}

inline constexpr
auto alloy::core::int_utilities::make_u16(s16 v)
  noexcept -> u16
{
  return make<u16>(v);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::int_utilities::make_u16(u16 v)
  noexcept -> u16
{
  return v;
}

inline constexpr
auto alloy::core::int_utilities::make_u16(u8 b0, u8 b1)
  noexcept -> u16
{
  return make<u16>(b0,b1);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::int_utilities::make_u32(s32 v)
  noexcept -> u32
{
  return make<u32>(v);
}

inline constexpr
auto alloy::core::int_utilities::make_u32(u32 v)
  noexcept -> u32
{
  return v;
}

inline constexpr
auto alloy::core::int_utilities::make_u32(u16 v0, u16 v1)
  noexcept -> u32
{
  return make<u32>(v0,v1);
}

inline constexpr
auto alloy::core::int_utilities::make_u32(u8 b0, u8 b1, u8 b2, u8 b3)
  noexcept -> u32
{
  return make<u32>(b0,b1,b2,b3);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::int_utilities::make_u64(s64 v)
  noexcept -> u64
{
  return static_cast<u64>(v);
}

inline constexpr
auto alloy::core::int_utilities::make_u64(u64 v)
  noexcept -> u64
{
  return v;
}

inline constexpr
auto alloy::core::int_utilities::make_u64(u32 v0, u32 v1)
  noexcept -> u64
{
  return make<u64>(v0,v1);
}

inline constexpr
auto alloy::core::int_utilities::make_u64(u16 v0, u16 v1, u16 v2, u16 v3)
  noexcept -> u64
{
  return make<u64>(v0,v1,v2,v3);
}

inline constexpr
auto alloy::core::int_utilities::make_u64(
  u8 b0, u8 b1, u8 b2, u8 b3,
  u8 b4, u8 b5, u8 b6, u8 b7
) noexcept -> u64
{
  return make<u64>(b0,b1,b2,b3,b4,b5,b6,b7);
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::int_utilities::equal_type::operator()(T lhs, U rhs)
  const noexcept -> bool
{
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
    return lhs == rhs;
  } else if constexpr (std::is_signed_v<T>) {
    using type = std::make_unsigned_t<T>;

    return lhs < 0 ? false : static_cast<type>(lhs) == rhs;
  } else {
    using type = std::make_unsigned_t<U>;

    return rhs < 0 ? false : lhs == static_cast<type>(rhs);
  }
}

template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::int_utilities::not_equal_type::operator()(T lhs, U rhs)
  const noexcept -> bool
{
  return !equal(lhs, rhs);
}

template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::int_utilities::greater_type::operator()(T lhs, U rhs)
  const noexcept -> bool
{
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<T>) {
    using type = std::make_unsigned_t<T>;

    return (lhs < 0) ? true : static_cast<type>(lhs) < rhs;
  } else {
    using type = std::make_unsigned_t<U>;

    return (rhs < 0) ? false : lhs < static_cast<type>(rhs);
  }
}

template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::int_utilities::greater_equal_type::operator()(T lhs, U rhs)
  const noexcept -> bool
{
  return !greater(rhs, lhs);
}

template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::int_utilities::less_type::operator()(T lhs, U rhs)
  const noexcept -> bool
{
  return greater(rhs, lhs);
}

template <typename T, typename U>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::int_utilities::less_equal_type::operator()(T lhs, U rhs)
  const noexcept -> bool
{
  return !greater(lhs, rhs);
}

//------------------------------------------------------------------------------
// Private: Splitting
//------------------------------------------------------------------------------

template <typename To, typename..., typename From>
inline constexpr
auto alloy::core::int_utilities::split(const From& from)
  noexcept -> std::array<To, sizeof(From) / sizeof(To)>
{
  return split_impl<To>(from,
    std::make_index_sequence<(sizeof(From) / sizeof(To))>{}
  );
}

template <typename To, typename OutputIt, typename From>
constexpr
auto alloy::core::int_utilities::split_into(OutputIt o, const From& from) noexcept -> void
{
  constexpr auto size = sizeof(From) / sizeof(To);

  split_into_impl<To>(o, from, std::make_index_sequence<size>{});
}

//------------------------------------------------------------------------------
// Extraction
//------------------------------------------------------------------------------

template <std::size_t N, typename To, typename From>
inline constexpr
auto alloy::core::int_utilities::extract(const From& from)
  noexcept -> To
{
  static_assert(
    sizeof(To) <= sizeof(From),
    "'To' must be at most the same size as 'From'"
  );
  static_assert(
    (sizeof(From) % sizeof(To)) == 0u,
    "'To' must be a multiple in size of 'From'"
  );
  static_assert(
    std::is_integral_v<To> || std::is_enum_v<To>,
    "'To' must be an integral or enum receiver type"
  );
  static_assert(
    std::is_integral_v<From> || std::is_enum_v<From>,
    "'From' must be an integral or enum receiver type"
  );

  constexpr auto indices = (sizeof(From) / sizeof(To));
  constexpr auto max_index = indices - 1u;

  static_assert(N < indices);

  // If we are extracting the last element, just cast -- no need to shift, as
  // this is already truncating bytes
  if constexpr (N == max_index) {
    return static_cast<To>(from);
  } else {
    using from_type = typename detail::unsigned_underlying_type<From>::type;

    constexpr auto shift = (max_index - N) * (sizeof(To) * 8u);
    constexpr auto mask = (from_type{1u} << shift) - 1u;

    return static_cast<To>((static_cast<from_type>(from) >> shift) & mask);
  }
}

//------------------------------------------------------------------------------
// Private Splitting
//------------------------------------------------------------------------------

template <typename To, std::size_t...Idxs, typename From>
inline constexpr
auto alloy::core::int_utilities::split_impl(
  const From& from, std::index_sequence<Idxs...>
) noexcept -> std::array<To, sizeof(From) / sizeof(To)>
{
  return {{extract<Idxs,To>(from)...}};
}

template <typename To, typename OutputIt, typename From, std::size_t... Idxs>
inline constexpr
auto alloy::core::int_utilities::split_into_impl(
  OutputIt o,
  const From& from,
  std::index_sequence<Idxs...>
) noexcept -> void
{
  ((*o = extract<Idxs,To>(from), ++o), ...);
}

#endif /* ALLOY_CORE_UTILITIES_INT_UTILITIES_HPP */
